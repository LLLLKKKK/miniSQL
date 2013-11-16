
#ifndef _SQLWORKER_H_
#define _SQLWORKER_H_

#include "common.h"
#include "logger/Logger.h"
#include "interpreter/SQLParser.h"
#include "buffermanager/BufferManager.h"
#include "catelogmanager/CatelogManager.h"
#include "analyzer/SQLAnalyzer.h"

namespace miniSQL {

class IndexManager;
class RecordManager;

typedef std::unique_ptr<RecordManager> RecordManagerPtr;
typedef std::unique_ptr<IndexManager> IndexManagerPtr;
typedef std::map<std::string, RecordManagerPtr> RecordManagerMap;
typedef std::map<std::string, IndexManagerPtr> IndexManagerMap;

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
    bool startCreate(ParseNodePtr node);
    bool startDrop(ParseNodePtr node);
    bool startSelect(ParseNodePtr node);
    bool startInsert(ParseNodePtr node);
    bool startDelete(ParseNodePtr node);

    bool checkCondition(const Record& record, const std::vector<ParseNodePtr>& condition);
private:
    BufferManagerPtr _bufferManager;
    CatelogManager _catelogManager;
    SQLAnalyzer _analyzer;
    RecordManagerMap _recordManagerMap;
    IndexManagerMap _indexManagerMap;

private:
    DECLARE_LOGGER(SQLWorker);
};

}

#endif
