
#ifndef _CATELOGMANAGER_H_
#define _CATELOGMANAGER_H_

#include <string>
#include <map>
#include <set>
#include "common.h"
#include "recordmanager/Record.h"
#include "logger/Logger.hpp"

namespace miniSQL {

typedef std::map<std::string, std::string> indexMap;

struct TableInfo {
    std::string name;
    RecordInfo recordInfo;
    std::map<std::string, std::string> indexMap;    
};

class CatelogManager {

    DISALLOW_COPY_AND_ASSIGN(CatelogManager);
public:
    bool init();
    
    bool serializePrimaryDataFile();
    bool deserializePrimaryDataFile();
    bool readTableInfoFile(const std::string& infoFile, TableInfo& tableInfo);
    bool writeTableInfoFile(const std::string& tablename, const TableInfo& tableInfo);
    
    
private:
    static const std::string PRIMARY_DATA_FILE;

private:
    std::set<std::string> _tableInfoFileSet;
    std::map<std::string, TableInfo> _tableMap;

private:
    DECLARE_LOGGER(CatelogManager);
};

}

#endif
