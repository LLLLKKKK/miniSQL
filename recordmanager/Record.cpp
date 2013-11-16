

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
        MINISQL_LOG_ERROR("[%s] can't fill in field!", fieldname.c_str());
        return ;
    }
    char* location = _recordBuffer + it->second.offset;    
    memcpy(location, field.c_str(), field.size());
}

template<>
void Record::getField(const std::string& fieldname, std::string& field) const {
    auto it = _recordInfo.fieldInfoMap.find(fieldname);
    assert(it != _recordInfo.fieldInfoMap.end());
    assert(it->second.offset + it->second.type.length <= _recordInfo.size);
    
    char* location = _recordBuffer + it->second.offset;
    field = std::string(location, it->second.type.length);
}

std::ostream& operator<<(std::ostream& os, const Record& record) {
    for (size_t i = 0; i < record._recordInfo.fields.size(); i++) {
        auto field = record._recordInfo.fields[i];
        auto it =  record._recordInfo.fieldInfoMap.find(field);
        auto type = it->second.type;
        os << "  ";
        switch (type.baseType) {
        case FloatType:
        {
            float f;
            record.getField(i, f);
            os << f;
        }
        case IntType:
        {
            int j;
            record.getField(i, j);
            os << j;
            break;
        }
        case CharType:
        {
            std::string c;
            record.getField(i, c);
            os << c;
            break;
        }
        default:
            break;
        }
    }
    return os;
}

}
