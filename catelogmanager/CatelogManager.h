
#ifndef _CATELOGMANAGER_H_
#define _CATELOGMANAGER_H_

#include <string>
#include <map>
#include <set>
#include "common.h"
#include "recordmanager/Record.h"
#include "logger/Logger.h"

namespace miniSQL {

typedef std::map<std::string, std::string> indexMap;

struct TableInfo {
    std::string name;
    RecordInfo recordInfo;
    std::map<std::string, std::string> indexMap;    
};

class CatelogManager {
public:
    CatelogManager();
    ~CatelogManager();

    DISALLOW_COPY_AND_ASSIGN(CatelogManager);
public:
    bool readCatelog();
    bool writeCatelog();
    bool addCatelog(const TableInfo& tableInfo);
    bool deleteCatelog(const std::string& tablename);
    bool getCatelog(const std::string& tablename, TableInfo& tableInfo);
    bool getIndex(const std::string& indexname);
    bool addIndex(const std::string& tablename, const std::string& indexname, 
                  const std::string& fieldname);
    bool deleteIndex(const std::string& indexname);
    
private:
    bool serializePrimaryDataFile();
    bool deserializePrimaryDataFile();
    bool readTableInfoFile(const std::string& tableFile, TableInfo& tableInfo);
    bool writeTableInfoFile(const std::string& tableFile, const TableInfo& tableInfo);
    
    
private:
    static const std::string PRIMARY_DATA_FILE;

private:
    std::set<std::string> _tableInfoFileSet;
    std::map<std::string, TableInfo> _tableMap;
    std::map<std::string, std::string> _indexMap;

private:
    DECLARE_LOGGER(CatelogManager);
};

}

#endif
