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

struct IndexInfo {
    std::string tablename;
    std::string columnname;
    std::string indexname;
};

class CatelogManager;

class SQLAnalyzer {
public:
    SQLAnalyzer(CatelogManager *catelogManager);
    DISALLOW_COPY_AND_ASSIGN(SQLAnalyzer);

public:
    bool validateCreateTable(ParseNodePtr node, TableInfo& tableInfo);
    bool validateCreateIndex(ParseNodePtr node, IndexInfo& indexInfo);
    bool validateDropTable(ParseNodePtr node, std::string& tablename);
    bool validateDropIndex(ParseNodePtr node, std::string& indexname);
    bool validateInsert(ParseNodePtr node, Record& record);
    bool validateSelect(ParseNodePtr node, TableInfo& tableInfo);
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
