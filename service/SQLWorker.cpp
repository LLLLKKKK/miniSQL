
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
        _recordManagerMap[tableInfo.name] = RecordManagerPtr(
                new RecordManager(_bufferManager, tableInfo.name + ".table", 
                        tableInfo.recordInfo));
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
        _catelogManager.addTable(tableInfo);
        _bufferManager->loadDbFile(tableInfo.name + ".table", true);
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
    if (!_analyzer.validateSelect(statement, tableInfo)) {
        MINISQL_LOG_ERROR("Validate select statement failed!");
        return false;
    }
    return true;
}

bool SQLWorker::startSelect(ParseNodePtr statement) {
    TableInfo tableInfo;
    if (!_analyzer.validateSelect(statement, tableInfo)) {
        MINISQL_LOG_ERROR("Validate select statement failed!");
        return false;
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
            return startCreate(statement);
        case DROP:
            return startDrop(statement);
        case SELECT:
            return startSelect(statement);
        case DELETE:
            return startDelete(statement);
        case INSERT:
            return startInsert(statement);
        default:
            return false;
        }
    }
    return true;
}

}
