#include "RecordManager.h"

namespace miniSQL {

RecordManager::RecordManager(BufferManagerPtr bufferManager, 
                             const std::string& recordFile,
                             const RecordInfo& recordInfo)
    : _bufferManager(bufferManager),
      _recordFile(recordFile),
      _recordInfo(recordInfo) 
{ }

RecordManager::~RecordManager() { }

bool RecordManager::init(bool isNew) {
    if (!_bufferManager->loadDbFile(_recordFile, isNew)) {
        MINISQL_LOG_ERROR("Fail to load record file [%s]!",
                          _recordFile.c_str());        
        return false;
    }

    PagePtr page;
    if (isNew) {
        page = createPage();
    } else {
        page = _bufferManager->getPage(_recordFile, PageID(1));
    }
    auto header = getRecordPageHeader(page);

    _nowPos = getFilePosition(header->header.my_id, header->record_start);
    if (getRecordMeta(_nowPos) != RECORD_EMPTY) {
        moveToNextPos(_nowPos, RECORD_EMPTY);
    }

    return true;
}

fileposition RecordManager::getRecordBeginPos(){
    auto page = _bufferManager->getPage(_recordFile, PageID(1));
    auto header = getRecordPageHeader(page);
    auto beginPos = getFilePosition(header->header.my_id, header->record_start);
    
    if (getRecordMeta(beginPos) != RECORD_VERSION) {
        moveToNextPos(beginPos, RECORD_VERSION);
    }
    return beginPos;
}

PagePtr RecordManager::createPage() {
    auto newPage = _bufferManager->createPage(_recordFile);
    if (!newPage) {
        MINISQL_LOG_ERROR("Fail to create page from record file [%s]!",
                          _recordFile.c_str());        
        return false;
    }
    newPage->isDirty = true;
    auto header = getRecordPageHeader(newPage);
    initRecordPageHeader(header);
    
    return newPage;
}

bool RecordManager::getRecord(const fileposition& pos, Record& record) {
    auto page = _bufferManager->getPage(_recordFile, getPageID(pos));
    if (!page) {
        MINISQL_LOG_ERROR("Invalid fileposition [%s] when reading record file [%s]!",
                          std::to_string(pos).c_str(), _recordFile.c_str());
        return false;
    }
    auto offset = getPageOffset(pos) + 4;
    record.loadFields(static_cast<char*>(page->data) + offset);
    return true;
}

void RecordManager::moveToNextPos(fileposition& pos, recordversion version) {
    if (pos == INVALID_FILEPOSITION) {
        return ;
    }

    auto pageID = getPageID(pos);
    auto offset = getPageOffset(pos);
    bool newPage = false;

    while (true) {
        PagePtr page;
        if (!_bufferManager->validatePage(_recordFile, pageID)) {
            if (version == RECORD_EMPTY) {
                page = createPage();
                pageID = page->id;
            } else {
                break;
            }
        } else {
            page = _bufferManager->getPage(_recordFile, pageID);
        }

        auto header = getRecordPageHeader(page);
        if (newPage) {
            offset = header->record_start;
        } else {
            offset += _recordInfo.size + 4;
        }

        pos = getFilePosition(pageID, offset);

        while (offset + _recordInfo.size + 4 <= header->record_slot_array_start) {
            if (getRecordMeta(pos) == version) {
                return ;
            }
            pos += _recordInfo.size + 4;
            offset += _recordInfo.size + 4;
        }

        newPage = true;
        pageID++;
    }

    pos = INVALID_FILEPOSITION;
    return ;
}

uint16_t RecordManager::getRecordMeta(fileposition& pos){
    auto page = _bufferManager->getPage(_recordFile, getPageID(pos));
    auto offset = getPageOffset(pos);
    auto now_ptr = static_cast<char*>(page->data) + offset;
    auto twobyte_ptr = reinterpret_cast<uint16_t*>(now_ptr);
    twobyte_ptr++;
        
    return *twobyte_ptr;
}

bool RecordManager::insertRecord(const Record& record, fileposition& pos) {
    auto page = _bufferManager->getPage(_recordFile, getPageID(pos));
    if (!page) {
        MINISQL_LOG_ERROR("Invalid fileposition [%s] when reading record file [%s]!",
                          std::to_string(pos).c_str(), _recordFile.c_str());
        return false;
    }

    auto header = getRecordPageHeader(page);
    auto offset = header->record_end;
    auto now_ptr = static_cast<char*>(page->data) + offset;
    auto twobyte_ptr = reinterpret_cast<uint16_t*>(now_ptr);
    *twobyte_ptr++ = RECORD_META;
    *twobyte_ptr++ = RECORD_VERSION;
    now_ptr = reinterpret_cast<char*>(twobyte_ptr);
    record.storeFields(now_ptr);
    header->record_end += _recordInfo.size + 4;
    
    return true;
}

bool RecordManager::insertRecord(const Record& record) {
    assert(_nowPos != INVALID_FILEPOSITION);
    bool ret = insertRecord(record, _nowPos);
    moveToNextPos(_nowPos, RECORD_EMPTY);

    return ret;
}


bool RecordManager::deleteRecord(const fileposition& position) {
    auto page = _bufferManager->getPage(_recordFile, getPageID(position));
    if (!page) {
        MINISQL_LOG_ERROR("Invalid fileposition [%s] when reading record file [%s]!",
                          std::to_string(position).c_str(), _recordFile.c_str());
        return false;
    }
    auto offset = getPageOffset(position);
    auto now_ptr = static_cast<char*>(page->data) + offset;
    auto twobyte_ptr = reinterpret_cast<uint16_t*>(now_ptr);
    twobyte_ptr++;
    *twobyte_ptr = RECORD_DELETED;

    return true;
}


void RecordManager::initRecordPageHeader(RecordPageHeader* header) {
    auto page_size = header->header.page_size;
    auto slot_count = (page_size - sizeof(RecordPageHeader)) / _recordInfo.size;
    header->slot_count = slot_count;
    header->free_count = header->slot_count;
    header->record_start = sizeof(RecordPageHeader);
    header->record_end = header->record_start;
    header->record_slot_array_start = page_size;
    header->record_slot_array_end = header->record_slot_array_start;
}

}
