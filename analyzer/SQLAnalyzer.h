#ifndef _SQLANALYZER_H_
#define _SQLANALYZER_H_

#include "logger/Logger.h"
#include "interpreter/SQLParser.h"
#include "catelogmanager/CatelogManager.h"
#include "recordmanager/Record.h"

namespace miniSQL {

enum AnalyzerError {
    TABLE_NAME_EXSITS,
    INVALID_TABLE_NAME,
    INDEX_NAME_EXSITS,
};

class CatelogManager;

class SQLAnalyzer {
public:
    SQLAnalyzer(CatelogManager *catelogManager);
    DISALLOW_COPY_AND_ASSIGN(SQLAnalyzer);

public:
    bool validateCreate(ParseNodePtr node, TableInfo& tableInfo);
    bool validateDrop(ParseNodePtr node);
    bool validateInsert(ParseNodePtr node, Record& record);
    bool validateSelect(ParseNodePtr node);
    bool validateDelete(ParseNodePtr node);

    AnalyzerError getLastError() const;
    
private:
    CatelogManager *_catelogManager;
    AnalyzerError _lastError;

private:
    DECLARE_LOGGER(SQLAnalyzer);
};

}

#endif
