#ifndef _SQLANALYZER_H_
#define _SQLANALYZER_H_

#include "Query.h"
#include "Command.h"
#include "interpreter/SQLParser.h"

namespace miniSQL {

enum AnalyzerError {
    TABLE_NAME_EXSITS,
    INVALID_TABLE_NAME,
    INDEX_NAME_EXSITS,
};


class SQLAnalyzer {
    DISALLOW_COPY_AND_ASSIGN(SQLAnalyzer);

public:
    std::vector<CommandPtr> analyze(std::list<ParseNodePtr> parseNodes);
    AnalyzerError getLastError() const;
};

}

#endif
