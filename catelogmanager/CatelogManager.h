
#ifndef _CATELOGMANAGER_H_
#define _CATELOGMANAGER_H_

#include <string>
#include <map>
#include <set>
#include "common.h"
#include "recordmanager/Record.h"
#include "logger/Logger.h"

namespace miniSQL {

struct TableInfo {
    std::string name;
    std::string primary;
    RecordInfo recordInfo;
    std::map<std::string, std::string> indexToColumnMap;
};

typedef std::map<std::string, std::string> IndexToTableMap;
typedef std::map<std::string, TableInfo> TableMap;


class CatelogManager {
public:
    CatelogManager();
    ~CatelogManager();

    DISALLOW_COPY_AND_ASSIGN(CatelogManager);
public:
    bool readTables();
    bool writeTables();
    bool addTable(const TableInfo& tableInfo);
    bool deleteTable(const std::string& tablename);
    bool getTable(const std::string& tablename, TableInfo& tableInfo);
    bool getIndex(const std::string& indexname);
    bool addIndex(const std::string& tablename, const std::string& indexname, 
                  const std::string& fieldname);
    bool deleteIndex(const std::string& indexname);

public:
    const IndexToTableMap& getIndexToTableMap() const {
        return _indexMap;
    }
    const TableMap& getTableMap() const {
        return _tableMap;
    }

private:
    bool serializePrimaryDataFile();
    bool deserializePrimaryDataFile();
    bool readTableInfoFile(const std::string& tableFile, TableInfo& tableInfo);
    bool writeTableInfoFile(const std::string& tableFile, const TableInfo& tableInfo);
    
    
private:
    static const std::string PRIMARY_DATA_FILE;

private:
    std::set<std::string> _tableInfoFileSet;
    TableMap _tableMap;
    IndexToTableMap _indexMap;

private:
    DECLARE_LOGGER(CatelogManager);
};

}

#endif
