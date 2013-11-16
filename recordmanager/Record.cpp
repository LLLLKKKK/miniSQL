

#include "Record.h"

namespace miniSQL {

Record::Record(const RecordInfo& recordInfo)
    :  _recordBuffer(new char[recordInfo.size]),
       _recordInfo(recordInfo) { }

Record::~Record() {
    if (_recordBuffer) {
        delete[] _recordBuffer;
    }
}

template<>
void Record::putField(const std::string& fieldname, const std::string& field) {
    auto it = _recordInfo.fieldInfoMap.find(fieldname);
    if (it == _recordInfo.fieldInfoMap.end()) {
        MINISQL_LOG_WARN("Record field [%s] not found", fieldname.c_str());
            return ;
    }
    if (field.size() != it->second.type.length || 
        it->second.offset + field.size() > _recordInfo.size) {
        MINISQL_LOG_ERROR("[%s] too big to fill in field!", fieldname.c_str());
        return ;
    }
    char* location = _recordBuffer + it->second.offset;    
    memcpy(location, field.c_str(), field.size());
}

template<>
void Record::getField(int pos, std::string& field) {
    if ((size_t)pos > _recordInfo.fields.size()) {
        MINISQL_LOG_ERROR("Field position [%d] is invalid!", pos);
    }
    auto fieldname = _recordInfo.fields[pos];
    auto it = _recordInfo.fieldInfoMap.find(fieldname);
    assert(it != _recordInfo.fieldInfoMap.end());
    assert(it->second.offset + it->second.type.length <= _recordInfo.size);
    
    char* location = _recordBuffer + it->second.offset;
    field = std::string(location, it->second.type.length);
}

}
