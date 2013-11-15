
#ifndef _RECORD_HPP_
#define _RECORD_HPP_

#include <string>
#include <map>
#include <cstring>
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
    uint32_t size;
};


class Record {
public:
    Record(const RecordInfo& recordInfo);
    ~Record();
    DISALLOW_COPY_AND_ASSIGN(Record);
    
    template<class T>
    void putField(const std::string& fieldname, const T& field);

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

};

#endif
