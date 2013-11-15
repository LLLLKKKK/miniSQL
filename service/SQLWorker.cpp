
#include <fstream>
#include <memory>
#include "SQLWorker.h"
#include "interpreter/InputHandler.h"
#include "interpreter/SQLScanner.h"

namespace miniSQL {

SQLWorker::SQLWorker() : 
    _bufferManager(nullptr),
    _analyzer(&_catelogManager)
    
{  }

SQLWorker::~SQLWorker() {
    _catelogManager.writeCatelog();
}

std::list<ParseNodePtr> SQLWorker::parse(std::istream* stream) {
    InputHandler inputHandler(stream);
    SQLScanner scanner(&inputHandler);
    SQLParser parser(&scanner);
    return parser.parseSQL();
}


bool SQLWorker::init() {
    _bufferManager = std::make_shared<BufferManager>();
    
    if (!_catelogManager.readCatelog()) {
        MINISQL_LOG_ERROR("CatelogManager init failed!");
        return false;
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
        _catelogManager.addCatelog(tableInfo);
        _bufferManager->loadDbFile(tableInfo.name, true);
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
        _bufferManager->loadDbFile(indexInfo.indexname);
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
        return _catelogManager.deleteCatelog(tablename);
    }
    // index
    else {
        std::string indexname;
        if (!_analyzer.validateDropIndex(statement, indexname)) {
            MINISQL_LOG_ERROR("Validate drop index statement failed!");
            return false;
        }
        return _catelogManager.deleteIndex(indexname);
    }
    return true;
}

bool SQLWorker::startSelect(ParseNodePtr statement) {
    TableInfo tableInfo;
    if (!_analyzer.validateSelect(statement, tableInfo)) {
        MINISQL_LOG_ERROR("Validate select statement failed!");
        return false;
    }
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
            return start(&file);
        }
        case CREATE:
            return startCreate(statement);
        case DROP:
            return startDrop(statement);
        case SELECT:
            return startSelect(statement);
            break;
        case DELETE:
            break;
        case INSERT:
            break;
    n    default:
            return false;
        }
    }
    return true;
}

}
