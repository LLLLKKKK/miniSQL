
#include <fstream>

#include "common.h"
#include "CatelogManager.h"

namespace miniSQL {


const std::string CatelogManager::PRIMARY_DATA_FILE = "miniSQL.primary";

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
    for (int i = 0; i < indexNum; i++) {
        std::string index, field;
        ifs >> index >> field;
        tableInfo.indexMap[index] = field;
    }
    int fieldNum;
    uint32_t size;
    ifs >> fieldNum >> size;
    while (fieldNum--) {
        std::string field;
        FieldType type;
        uint32_t offset;
        ifs >> field >> type.type >> offset;
        tableInfo.recordInfo.fieldInfoMap[field] = { type, offset };
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
    ofs << (int) tableInfo.indexMap.size() << '\n';
    for (auto it = tableInfo.indexMap.begin(); it != tableInfo.indexMap.end(); it++) {
        ofs << it->first << ' ' << it->second << '\n';
    }
    ofs << (int) tableInfo.recordInfo.fieldInfoMap.size() << tableInfo.recordInfo.size;
    for (auto it = tableInfo.recordInfo.fieldInfoMap.begin();
         it != tableInfo.recordInfo.fieldInfoMap.end(); it++) {
        ofs << it->first << ' ' << it->second.type.type << ' ' << it->second.offset << '\n';
    }
    return true;
}

}
