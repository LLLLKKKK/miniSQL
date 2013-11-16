
#ifndef _RECORD_HPP_
#define _RECORD_HPP_

#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <cassert>
#include "common.h"
#include "logger/Logger.h"

namespace miniSQL {

typedef uint8_t FieldBaseType;

const FieldBaseType FloatType = 0;
const FieldBaseType IntType = 1;
const FieldBaseType CharType = 2;

const FieldBaseType UniqueType = 1 << 7;

struct FieldType {
    FieldBaseType baseType;
    uint8_t length;
};

struct FieldInfo {
    FieldType type;
    uint32_t offset;
};

typedef std::map<std::string, FieldInfo> FieldInfoMap;

struct RecordInfo {
    FieldInfoMap fieldInfoMap;
    std::vector<std::string> fields;
    uint32_t size;
};


class Record {
public:
    Record(const RecordInfo& recordInfo);
    ~Record();
    DISALLOW_COPY_AND_ASSIGN(Record);
    
    template<class T>
    void putField(const std::string& fieldname, const T& field) {
        auto it = _recordInfo.fieldInfoMap.find(fieldname);
        if (it == _recordInfo.fieldInfoMap.end()) {
            MINISQL_LOG_WARN("Record field [%s] not found", fieldname.c_str());
            return ;
        }
        if (it->second.offset + sizeof(T) > _recordInfo.size) {
            MINISQL_LOG_ERROR("[%s] too big to fill in field!", fieldname.c_str());
            return ;
        }
        char* location = _recordBuffer + it->second.offset;
        *(reinterpret_cast<T*>(location)) = field;
    }


    template<class T>
    void putField(int pos, const T& field) {
        if (pos > _recordInfo.fields.size()) {
            MINISQL_LOG_ERROR("Field position [%d] is invalid!", pos);
        }
        auto fieldname = _recordInfo.fields[pos];
        putField(fieldname, field);
    }

    template<class T>
    void getField(int pos, T& field) {
        if (pos > _recordInfo.fields.size()) {
            MINISQL_LOG_ERROR("Field position [%d] is invalid!", pos);
        }
        auto fieldname = _recordInfo.fields[pos];
        auto it = _recordInfo.fieldInfoMap.find(fieldname);
        assert(it != _recordInfo.fieldInfoMap.end());
        assert(it->second.offset + sizeof(T) <= _recordInfo.size);
        
        char* location = _recordBuffer + it->second.offset;
        field = *(reinterpret_cast<T*>(location));
    }

    void loadFields(const void* memory) {
        memcpy(_recordBuffer, memory, _recordInfo.size);
    }
    
    void storeFields(void* memory) const {
        memcpy(memory, _recordBuffer, _recordInfo.size);
    }
    
private:
    char* _recordBuffer;
    RecordInfo _recordInfo;
    DECLARE_LOGGER(Record);
};

template<>
void Record::putField(const std::string& fieldname, const std::string& field);

template<>
void Record::getField(int pos, std::string& field);

};

#endif
