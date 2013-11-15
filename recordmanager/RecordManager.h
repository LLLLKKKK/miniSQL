
#ifndef _RECORDMANAGER_H_
#define _RECORDMANAGER_H_

#include <string>
#include <set>

#include "common.h"
#include "logger/Logger.h"
#include "buffermanager/Page.h"
#include "buffermanager/BufferManager.h"
#include "Record.h"

namespace miniSQL {

typedef uint64_t fileposition;

struct __attribute__((packed)) RecordPageHeader {
    struct PageHeader header;
    uint32_t slot_count;
    uint32_t free_count;
    
    uint32_t record_start;
    uint32_t record_end;

    uint32_t record_slot_array_start;
    uint32_t record_slot_array_end;
};


class RecordManager {
public: 
    RecordManager(BufferManagerPtr _bufferManager, 
                  const std::string& recordFile,
                  const RecordInfo& recordInfo);
    ~RecordManager();
public:
    bool init(bool isNew = false);
    
    bool getRecord(const fileposition& position, 
                   Record& record);
    bool insertRecord(const Record& record, fileposition& pos);
    bool deleteRecord(const fileposition& position);

private:
    PageID getPageID(fileposition pos) {
        return static_cast<uint32_t>(pos >> 32);
    }
    
    uint32_t getPageOffset(fileposition pos) {
    return static_cast<uint32_t>(pos);
    }
    
    RecordPageHeader* getRecordPageHeader() {
        return reinterpret_cast<RecordPageHeader*>(_currentPage->data);
    }
    
    void initRecordPageHeader(RecordPageHeader* header);
private:
    BufferManagerPtr _bufferManager;
    std::string _recordFile;
    RecordInfo _recordInfo;
    PagePtr _currentPage;

private:
    DECLARE_LOGGER(RecordManager);
};

}

#endif
