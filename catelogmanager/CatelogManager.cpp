
#include <fstream>
#include <iostream>
#include "common.h"
#include "CatelogManager.h"

namespace miniSQL {

const std::string CatelogManager::PRIMARY_DATA_FILE = "miniSQL.primary";

CatelogManager::CatelogManager() { 

}

CatelogManager::~CatelogManager() {
    writeCatelog();
}

bool CatelogManager::writeCatelog() {
    if (!serializePrimaryDataFile()) {
        MINISQL_LOG_ERROR("Serialize primary data file [%s] failed!", 
                          PRIMARY_DATA_FILE.c_str());
        return false;
    }
    for (auto& table : _tableMap) {
        if (!writeTableInfoFile(table.first, table.second)) {
            MINISQL_LOG_ERROR("Write table info file [%s] failed!", table.first.c_str());
            return false;
        }
    }
    return true;
}

bool CatelogManager::readCatelog() {
    if (!deserializePrimaryDataFile()) {
        MINISQL_LOG_ERROR("Deserialize primary data file [%s] failed!", 
                          PRIMARY_DATA_FILE.c_str());
        return false;
    }
    for (auto& tableFile : _tableInfoFileSet) {
        TableInfo tableInfo;
        if (!readTableInfoFile(tableFile, tableInfo)) {
            MINISQL_LOG_ERROR("Read table info file [%s] failed!", tableFile.c_str());
            return false;
        }
        _tableMap[tableInfo.name] = tableInfo;
    }
    return true;
}

bool CatelogManager::addCatelog(const TableInfo& tableInfo) {
    if (_tableInfoFileSet.find(tableInfo.name) != _tableInfoFileSet.end()) {
        MINISQL_LOG_ERROR("Table [%s] has already exists!",
                          tableInfo.name.c_str());
        return false;
    }
    if (_tableMap.find(tableInfo.name) != _tableMap.end()) {
        MINISQL_LOG_ERROR("Table [%s] has already exists!",
                          tableInfo.name.c_str());
        return false;
    }
    _tableInfoFileSet.insert(tableInfo.name);
    _tableMap[tableInfo.name] = tableInfo;

    return true;
}

bool CatelogManager::deleteCatelog(const std::string& tablename) {
    if (_tableInfoFileSet.find(tablename) == _tableInfoFileSet.end()) {
        MINISQL_LOG_ERROR("Table [%s] does not exists!",
                          tablename.c_str());
        return false;
    }
    if (_tableMap.find(tablename) == _tableMap.end()) {
        MINISQL_LOG_ERROR("Table [%s] does not exists!",
                          tablename.c_str());
        return false;
    }
    _tableInfoFileSet.erase(tablename);
    _tableMap.erase(tablename);

    return true;
}

bool CatelogManager::getCatelog(const std::string& tablename, TableInfo& tableInfo) {
    if (_tableInfoFileSet.find(tablename) == _tableInfoFileSet.end()) {
        MINISQL_LOG_ERROR("Table [%s] does not exists!",
                          tablename.c_str());
        return false;
    }
    if (_tableMap.find(tablename) == _tableMap.end()) {
        MINISQL_LOG_ERROR("Table [%s] does not exists!",
                          tablename.c_str());
        return false;
    }
    tableInfo = _tableMap[tablename];

    return true;
}

// For serialize and deserizlize primary data file
// well, just to make it easy
// 1. 1st line: number of files
// 2. then each line for a single file, and each file for a table

bool CatelogManager::serializePrimaryDataFile() {
    std::ofstream ofs;
    ofs.open(CatelogManager::PRIMARY_DATA_FILE, std::ofstream::out | std::ofstream::trunc);
    if (! ofs.good() ) {
        MINISQL_LOG_ERROR( "Open primary data file %s for writing failed!", 
                          PRIMARY_DATA_FILE.c_str());
        return false;
    }
    ofs << (int) _tableInfoFileSet.size() << '\n';
    for (auto& table : _tableInfoFileSet) {
        ofs << table << '\n';
    }
    
    return true;
}

bool CatelogManager::deserializePrimaryDataFile() {
    std::ifstream ifs;
    ifs.open(PRIMARY_DATA_FILE, std::ifstream::in);
    if (! ifs.good() ) {
        MINISQL_LOG_ERROR( "open primary data file %s for reading failed!", 
                          PRIMARY_DATA_FILE.c_str());
        return false;
    }

    int numTable;
    ifs >> numTable;
    while (numTable--) {
        std::string table;
        ifs >> table;
        _tableInfoFileSet.insert(table);
    }

    return true;
}

bool CatelogManager::readTableInfoFile(const std::string& infoFile, TableInfo& tableInfo) {
    std::ifstream ifs;
    ifs.open(infoFile, std::ifstream::in);
    if (! ifs.good() ) {
        MINISQL_LOG_ERROR( "Open table info file %s for reading failed!", 
                          infoFile.c_str());
        return false;
    }
    ifs >> tableInfo.name;
    int indexNum;
    ifs >> indexNum;
    while (indexNum--) {
        std::string index, field;
        ifs >> index >> field;
        tableInfo.indexMap[index] = field;
        _indexMap[index] = tableInfo.name;
    }
    int fieldNum;
    uint32_t size;
    ifs >> fieldNum >> size;
    tableInfo.recordInfo.size = size;
    while (fieldNum--) {
        std::string field;
        int baseType, length;
        uint32_t offset;
        ifs >> field >> baseType >> length >> offset;
        FieldType type { static_cast<FieldBaseType>(baseType), static_cast<uint8_t>(length) };
        tableInfo.recordInfo.fieldInfoMap[field] ={ type, offset };
    }
    return true;
}

bool CatelogManager::writeTableInfoFile(const std::string& infoFile, 
                                        const TableInfo& tableInfo) {
    std::ofstream ofs;
    ofs.open(infoFile, std::ofstream::out | std::ofstream::trunc);
    if (! ofs.good() ) {
        MINISQL_LOG_ERROR( "Open table info file %s for writing failed!", 
                          infoFile.c_str());
        return false;
    }
    ofs << tableInfo.name << '\n';
    ofs << tableInfo.indexMap.size() << '\n';
    for (auto it = tableInfo.indexMap.begin(); it != tableInfo.indexMap.end(); it++) {
        ofs << it->first << ' ' << it->second << '\n';
    }

    ofs << tableInfo.recordInfo.fieldInfoMap.size() << ' '
        << tableInfo.recordInfo.size << '\n';

    for (auto it = tableInfo.recordInfo.fieldInfoMap.begin();
         it != tableInfo.recordInfo.fieldInfoMap.end(); it++) {
        ofs << it->first << ' ' 
            << (int) it->second.type.baseType << ' ' 
            << (int) it->second.type.length << ' ' 
            << it->second.offset << '\n';
    }
    return true;
}

bool CatelogManager::getIndex(const std::string& indexname) {
    return _indexMap.find(indexname) != _indexMap.end();
}

bool CatelogManager::addIndex(const std::string& tablename, const std::string& indexname,
                              const std::string& fieldname) {
    if (getIndex(indexname)) {
        MINISQL_LOG_ERROR( "Index [%s] has been existed!", indexname.c_str());
        return false;
    }
    auto it = _tableMap.find(tablename);
    if (it == _tableMap.end()) {
        MINISQL_LOG_ERROR( "Table [%s] does not exist!", tablename.c_str());
        return false;
    }
    it->second.indexMap[indexname] = fieldname;
    _indexMap[indexname] = tablename;
    return false;
}

bool CatelogManager::deleteIndex(const std::string& indexname) {
    auto it = _indexMap.find(indexname);
    if (it == _indexMap.end()) {
        MINISQL_LOG_ERROR( "Index [%s] has been existed!", indexname.c_str());
        return false;
    }
    auto tIt = _tableMap.find(it->second);
    if (tIt == _tableMap.end()) {
        MINISQL_LOG_ERROR( "Table [%s] does not exist!", it->first.c_str());
        return false;
    }
    tIt->second.indexMap.erase(tIt->second.indexMap.find(indexname));

    return true;
}

}
