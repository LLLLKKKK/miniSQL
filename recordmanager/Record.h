
#ifndef _RECORD_HPP_
#define _RECORD_HPP_

#include <string>
#include <map>
#include "common.h"
#include "logger/Logger.hpp"

namespace miniSQL {

typedef uint8_t FieldBaseType;

const FieldBaseType FloatType = 0;
const FieldBaseType IntType = 1;
const FieldBaseType CharType = 2;

const FieldBaseType UniqueType = 1 << 7;

union FieldType {
    struct {
        FieldBaseType baseType;
        uint8_t length;
    };
    uint16_t type;
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
    
private:
    char* _recordBuffer;
    RecordInfo _recordInfo;
    DECLARE_LOGGER(Record);
};

};

#endif
