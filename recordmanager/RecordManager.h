
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

const uint16_t RECORD_META = 0x1234;

typedef uint16_t recordversion;

const recordversion RECORD_EMPTY = 0x0000;
const recordversion RECORD_VERSION = 0x0001;
const recordversion RECORD_DELETED = 0x8000;

typedef uint64_t fileposition;

const fileposition INVALID_FILEPOSITION = 0;

struct __attribute__((packed)) RecordPageHeader {
    struct PageHeader header;
    uint32_t slot_count;
    uint32_t free_count;
    
    uint32_t record_start;
    uint32_t record_end;

    uint32_t record_slot_array_start;
    uint32_t record_slot_array_end;
};

class RecordManager;

class RecordManager {
public: 
    RecordManager(BufferManagerPtr _bufferManager, 
                  const std::string& recordFile,
                  const RecordInfo& recordInfo);
    ~RecordManager();
public:
    bool init(bool isNew = false);
    
    fileposition getRecordBeginPos();
    bool getRecord(const fileposition& position, Record& record);
    bool insertRecord(const Record& record);
    bool deleteRecord(const fileposition& position);

    void moveToNextPos(fileposition& pos, recordversion version);

private:
    PageID getPageID(fileposition pos) {
        return static_cast<uint32_t>(pos >> 32);
    }
    
    uint32_t getPageOffset(fileposition pos) {
        return static_cast<uint32_t>(pos);
    }

    fileposition getFilePosition(PageID id, uint32_t offset) {
        return (static_cast<uint64_t>(id) << 32) + offset;
    }

    RecordPageHeader* getRecordPageHeader(PagePtr page) {
        return reinterpret_cast<RecordPageHeader*>(page->data);
    }

    uint16_t getRecordMeta(fileposition& pos);
    void initRecordPageHeader(RecordPageHeader* header);
    bool insertRecord(const Record& record, fileposition& pos);

    PagePtr createPage();

private:
    BufferManagerPtr _bufferManager;
    std::string _recordFile;
    RecordInfo _recordInfo;
    fileposition _nowPos;
    
private:
    DECLARE_LOGGER(RecordManager);
};

}

#endif
