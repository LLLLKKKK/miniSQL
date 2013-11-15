

#include "Record.h"
#include <cassert>
#include <cstring>

namespace miniSQL {

Record::Record(const RecordInfo& recordInfo)
    :  _recordBuffer(new char[recordInfo.size]),
       _recordInfo(recordInfo) { }

Record::~Record() {
    if (_recordBuffer) {
        delete[] _recordBuffer;
    }
}

template<class T>
void Record::putField(const std::string& fieldname, const T& field) {
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

template<>
void Record::putField(const std::string& fieldname, const std::string& field) {
    auto it = _recordInfo.fieldInfoMap.find(fieldname);
    if (it == _recordInfo.fieldInfoMap.end()) {
        MINISQL_LOG_WARN("Record field [%s] not found", fieldname.c_str());
        return ;
    }
    if (it->second.offset + field.size() > _recordInfo.size) {
        MINISQL_LOG_ERROR("[%s] too big to fill in field!", fieldname.c_str());
        return ;
    }
    char* location = _recordBuffer + it->second.offset;    
    memcpy(location, field.c_str(), field.size());
}


}
