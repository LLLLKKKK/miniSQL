
#include <cstdio>
#include <fstream>
#include <memory>
#include "SQLWorker.h"
#include "interpreter/InputHandler.h"
#include "interpreter/SQLScanner.h"
#include "recordmanager/RecordManager.h"
#include "indexmanager/IndexManager.h"

namespace miniSQL {

SQLWorker::SQLWorker() : 
    _bufferManager(nullptr),
    _analyzer(&_catelogManager)
    
{  }

SQLWorker::~SQLWorker() {
    _catelogManager.writeTables();
}

std::list<ParseNodePtr> SQLWorker::parse(std::istream* stream) {
    InputHandler inputHandler(stream);
    SQLScanner scanner(&inputHandler);
    SQLParser parser(&scanner);
    auto statements = parser.parseSQL();
    if (parser.isError()) {
        return std::list<ParseNodePtr>();
    } else {
        return statements;
    }
}


bool SQLWorker::init() {
    _bufferManager = std::make_shared<BufferManager>();
    _bufferManager->init();

    if (!_catelogManager.readTables()) {
        MINISQL_LOG_ERROR("CatelogManager init failed!");
        return false;
    }
    
    auto tableMap = _catelogManager.getTableMap();
    for (auto& table : tableMap) {
        auto tableInfo = table.second;
        auto recordManager = RecordManagerPtr(new RecordManager
                (_bufferManager, tableInfo.name + ".table", tableInfo.recordInfo));
        recordManager->init();
        _recordManagerMap[tableInfo.name] = std::move(recordManager);
    }
    
    auto indexToTableMap = _catelogManager.getIndexToTableMap();
    for (auto& index : indexToTableMap) {
        auto fieldInfo = tableMap[index.second].recordInfo.fieldInfoMap[index.first];
        _indexManagerMap[index.first] = IndexManagerPtr(
                new IndexManager(_bufferManager, index.first + ".index", fieldInfo));
    }

    return true;
}

bool SQLWorker::startCreate(ParseNodePtr statement) {
    // table
    if (statement->children.front()->token == TABLE) {
        TableInfo tableInfo;
        if (!_analyzer.validateCreateTable(statement, tableInfo)) {
            MINISQL_LOG_ERROR("Validate create table statement failed!");
            return false;
        }
        auto recordManager = RecordManagerPtr(new RecordManager
                (_bufferManager, tableInfo.name + ".table", tableInfo.recordInfo));
        if (recordManager->init(true)) {
            _catelogManager.addTable(tableInfo);
        } else {
            MINISQL_LOG_ERROR("Init Record Manger failed!");
            return false;
        }
        _recordManagerMap[tableInfo.name] = std::move(recordManager);
    }
    // index
    else {
        IndexInfo indexInfo;
        if (!_analyzer.validateCreateIndex(statement, indexInfo)) {
            MINISQL_LOG_ERROR("Validate create index statement failed!");
            return false;
        }
        _catelogManager.addIndex(indexInfo.tablename, indexInfo.indexname, 
                indexInfo.columnname);
        _bufferManager->loadDbFile(indexInfo.indexname + ".index");
        // index building...
    }
    return true;
}

bool SQLWorker::startDrop(ParseNodePtr statement) {
    // table
    if (statement->children.front()->token == TABLE) {
        std::string tablename;
        if (!_analyzer.validateDropTable(statement, tablename)) {
            MINISQL_LOG_ERROR("Validate drop table statement failed!");
            return false;
        }
        std::remove(tablename.c_str());
        std::remove((tablename + ".table").c_str());
        TableInfo tableInfo;
        _catelogManager.getTable(tablename, tableInfo);
        for (auto index : tableInfo.indexToColumnMap) {
            std::remove((index.first + ".index").c_str());
        }
        return _catelogManager.deleteTable(tablename);
    }
    // index
    else {
        std::string indexname;
        if (!_analyzer.validateDropIndex(statement, indexname)) {
            MINISQL_LOG_ERROR("Validate drop index statement failed!");
            return false;
        }
        std::remove((indexname + ".index").c_str());
        return _catelogManager.deleteIndex(indexname);
    }
    return true;
}

bool SQLWorker::startInsert(ParseNodePtr statement) {
    auto tablename = static_cast<IdentifierNode*>(statement->children[0].get())->id;
    TableInfo tableInfo;
    if (!_catelogManager.getTable(tablename, tableInfo)) {
        MINISQL_LOG_ERROR("Table [%s] does not exist!", tablename.c_str());
        return false;
    }
    Record record(tableInfo.recordInfo);
    if (!_analyzer.validateInsert(statement, tableInfo, record)) {
        MINISQL_LOG_ERROR("Validate insert statement failed!");
        return false;
    }
    return _recordManagerMap[tablename]->insertRecord(record);
}

bool SQLWorker::startDelete(ParseNodePtr statement) {
    TableInfo tableInfo;
    std::vector<ParseNodePtr> condition;
    if (!_analyzer.validateSelect(statement, tableInfo, condition)) {
        MINISQL_LOG_ERROR("Validate select statement failed!");
        return false;
    }
    return true;
}

bool SQLWorker::startSelect(ParseNodePtr statement) {
    TableInfo tableInfo;
    std::vector<ParseNodePtr> condition;
    if (!_analyzer.validateSelect(statement, tableInfo, condition)) {
        MINISQL_LOG_ERROR("Validate select statement failed!");
        return false;
    }
    auto& recordManager = _recordManagerMap[tableInfo.name];
    auto recordPos = recordManager->getRecordBeginPos();
    Record record(tableInfo.recordInfo);
    while (recordPos != INVALID_FILEPOSITION) {
        recordManager->getRecord(recordPos, record);
        if (checkCondition(record, condition)) {
            std::cout << record << '\n';
        }
        recordManager->moveToNextPos(recordPos, RECORD_VERSION);
    }
    return true;
}

bool SQLWorker::start(std::istream* stream) {
    auto statements = parse(stream);
    for (auto& statement : statements) {
        switch (statement->token) {
        case QUIT:
            return false;
        case EXECFILE: 
        {            
            std::string filename =
                (static_cast<IdentifierNode*>(statement->children.front().get()))->id;
            std::ifstream file(filename, std::ifstream::in);
            if (!file.good()) {
                MINISQL_LOG_ERROR("Bad sql file!");
                return true;
            }
            return start(&file);
        }
        case CREATE:
            startCreate(statement);
            break;
        case DROP:
            startDrop(statement);
            break;
        case SELECT:
            startSelect(statement);
            break;
        case DELETE:
            startDelete(statement);
            break;
        case INSERT:
            startInsert(statement);
            break;
        default:
            return false;
        }
    }
    return true;
}


template<class T>
static bool Compare(const T& a, const T& b, Token token) {
    switch (token) {
    case GREATER:
        return a > b;
    case LESS:
        return a < b;
    case GREATER_EQUAL:
        return a >= b;
    case LESS_EQUAL:
        return a <= b;
    case EQUAL:
        return a == b;
    case NOT_EQUAL:
        return a != b;
    default:
        return false;
    }
}

bool SQLWorker::checkCondition(const Record& record, 
                               const std::vector<ParseNodePtr>& condition){
    for (auto& cond : condition) {
        auto field = static_cast<IdentifierNode*>(cond->children[0].get())->id;
        auto oprand = cond->token;
        auto value = cond->children[1];
        auto recordInfo = record.getRecordInfo();
        auto it = recordInfo.fieldInfoMap.find(field);
        switch(it->second.type.baseType) {
        case FloatType:
        {
            auto compValue = static_cast<FloatNode*>(value.get())->f_;
            float recordValue;
            record.getField(field, recordValue);
            if (! Compare(recordValue, compValue, oprand)) return false;
            break;
        }
        case IntType:
        {
            auto compValue = static_cast<IntNode*>(value.get())->i;
            int recordValue;
            record.getField(field, recordValue);
            if (! Compare(recordValue, compValue, oprand)) return false;
            break;
        }
        case CharType:
        {
            auto compValue = static_cast<CharNode*>(value.get())->c_;
            std::string recordValue;
            record.getField(field, recordValue);
            if (! Compare(recordValue, compValue, oprand)) return false;
            break;
        }
        }
    }
    return true;
}

}
