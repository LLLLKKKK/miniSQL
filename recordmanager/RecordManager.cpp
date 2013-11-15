
#include "RecordManager.h"

namespace miniSQL {

RecordManager::RecordManager(BufferManagerPtr bufferManager, 
                             const std::string& recordFile,
                             const RecordInfo& recordInfo)
    : _bufferManager(bufferManager),
      _recordFile(recordFile),
      _recordInfo(recordInfo),
      _currentPage(nullptr) { } ;

RecordManager::~RecordManager() {

}


bool RecordManager::init(bool isNew) {
    if (!_bufferManager->loadDbFile(_recordFile, isNew)) {
        MINISQL_LOG_ERROR("Fail to load record file [%s]!",
                          _recordFile.c_str());        
        return false;
    }
    _currentPage = _bufferManager->createPage(_recordFile);
    if (!_currentPage) {
        MINISQL_LOG_ERROR("Fail to create page from record file [%s]!",
                          _recordFile.c_str());        
        return false;
    }
    return true;
}

bool RecordManager::getRecord(const fileposition& pos, Record& record) {
    auto page = _bufferManager->getPage(_recordFile, getPageID(pos));
    if (!page) {
        MINISQL_LOG_ERROR("Invalid fileposition [%s] when reading record file [%s]!",
                          std::to_string(pos).c_str(), _recordFile.c_str());
        return false;
    }
    auto offset = getPageOffset(pos);
    record.loadFields(static_cast<char*>(page->data) + offset);
    return true;
}

bool RecordManager::insertRecord(const Record& record, fileposition& pos) {
    if (!_currentPage) {
        MINISQL_LOG_ERROR("Fail to create page from record file [%s]!",
                          _recordFile.c_str());        
        return false;
    }
    auto header = getRecordPageHeader();
    auto offset = header->record_end;
    auto now_ptr = static_cast<char*>(_currentPage->data) + offset;
    auto 2byte_ptr = reinterpret_cast<uint16_t*>(now_ptr);
    *2byte_ptr++ = RECORD_META;
    *2byte_ptr++ = RECORD_VERSION;
    now_ptr = reinterpret_cast<char*>(2byte_ptr);
    record.storeFields(now_ptr);
    header->record_end += _recordInfo.size + 4;
    
    if (header->record_end + _recordInfo.size > header->record_slot_array_start) {
        _currentPage = _bufferManager->createPage(_recordFile);
    }

    return true;
}

bool RecordManager::deleteRecord(const fileposition& position) {
    auto page = _bufferManager->getPage(_recordFile, getPageID(pos));
    if (!page) {
        MINISQL_LOG_ERROR("Invalid fileposition [%s] when reading record file [%s]!",
                          std::to_string(pos).c_str(), _recordFile.c_str());
        return false;
    }
    auto offset = getPageOffset(pos);
    auto now_ptr = static_cast<char*>(_currentPage->data) + offset;
    auto 2byte_ptr = reinterpret_cast<uint32_t*>(now_ptr);
    2byte_ptr++;
    2byte_ptr = RECORD_DELETED;

    return true;
}


void RecordManager::initRecordPageHeader(RecordPageHeader* header) {
    auto page_size = header->header.page_size;
    auto slot_count = (page_size - sizeof(RecordPageHeader)) / _recordInfo.size;
    header->slot_count = slot_count;
    header->free_count = header->slot_count;
    header->record_start = sizeof(RecordPageHeader);
    header->record_end = header->record_end;
    header->record_slot_array_start = page_size;
    header->record_slot_array_start = header->record_slot_array_end;   
}

}
