
#ifndef _SQLWORKER_H_
#define _SQLWORKER_H_

#include "common.h"
#include "logger/Logger.h"
#include "interpreter/SQLParser.h"
#include "buffermanager/BufferManager.h"
#include "recordmanager/RecordManager.h"
#include "catelogmanager/CatelogManager.h"

namespace miniSQL {

class SQLWorker {
public:
    SQLWorker();
    ~SQLWorker();    

    DISALLOW_COPY_AND_ASSIGN(SQLWorker);
public:
    bool init();
    bool start(std::istream* stream);
    
private:
    std::list<ParseNodePtr> parse(std::istream* stream);
private:
    BufferManagerPtr _bufferManager;
    CatelogManager _catelogManager;

private:
    DECLARE_LOGGER(SQLWorker);
};

}

#endif
