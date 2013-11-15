
#include <fstream>
#include "SQLWorker.h"
#include "interpreter/InputHandler.h"
#include "interpreter/SQLScanner.h"

namespace miniSQL {

SQLWorker::SQLWorker() : 
    _bufferManager(nullptr)
    
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
            break;
        case DROP:
            break;
        case SELECT:
            break;
        case DELETE:
            break;
        case INSERT:
            break;
        default:
            return false;
        }
    }
    return true;
}

}
