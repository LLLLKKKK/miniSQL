
#include <string>
#include "buffermanager/BufferManager.h"
#include "recordmanager/RecordManager.h"
#include "recordmanager/Record.h"

namespace miniSQL {

template<class T>
struct Deserial {
    T operator() (void* mem, uint32_t size, const T&) {
        return *(static_cast<T*>(mem));
    }
};

template<>
struct Deserial<std::string> {
    std::string operator() (void* mem, uint32_t size, const std::string&) {
        return std::string(static_cast<char*>(mem), static_cast<size_t>(size));
    }
};

enum Comp {
    EQUAL,
    GREATER,
    LESS,
    GREATER_EQUAL,
    LESS_EUQAL,
};

class IndexManager {
public:
    IndexManager(BufferManagerPtr bufferManager, 
                 const std::string indexFile,
                 const FieldInfo& fieldInfo);
    ~IndexManager();
public:
    template<class T>
    bool insertIndex(const T& key, const fileposition& pos);
    
    template<class T>
    bool deleteIndex(const T& key);
    
    template<class T, class Deserial>
    std::pair<RecordIterator, RecordIterator> search(const T& key, Comp comp);
    
private:
    BufferManagerPtr _bufferManager;

private:
    DECLARE_LOGGER(IndexManager);
};

};
