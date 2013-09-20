
#ifndef BUFFERMANAGER_HPP
#define BUFFERMANAGER_HPP

#include <map>
#include <string>
#include <cassert>
#include "common.h"
#include "LRUCache.hpp"
#include "Page.hpp"

namespace miniSQL {

class FixedSizeChunkAllocator;
class BufferManager;
class Logger;

class BufferManager {

public:
    BufferManager(LRUCache<PageID, PagePtr, PageGetSizeCallback>* cache, 
                  FixedSizeChunkAllocator* allocator, 
                  const std::string& primaryDataFile);
    ~BufferManager();

    DISALLOW_COPY_AND_ASSIGN(BufferManager);

public:
    bool init();

    PageID createPage(PageID pageID);
    bool pinPage(PageID PageID);
    bool unpinPage(PageID PageID);
    bool deletePage(PageID PageID);
    PagePtr getPage(PageID pageID);

private:
    DbFilePtr createDbFile(const std::string& filename);
    DbFilePtr loadDbFile(const std::string& filename);

    PagePtr mapPage(PageID);
    bool unmapPage(Page*);

    void initDbFileHeaderPage(PagePtr);
    void initNormalPage(PagePtr);

    bool serializePrimaryDataFile();
    bool deserializePrimaryDataFileAndLoad();

private:

    uint32_t _maxDbFileSize;
    uint32_t _pageSize;

    LRUCache<PageID, PagePtr, PageGetSizeCallback> *_cache;
    FixedSizeChunkAllocator *_allocator;
    const std::string _primaryDataFile;

    std::map<FileID, DbFilePtr> _fileMap;
//    std::map<FileID, DbFilePtr> _nonfullFileMap;
    
//    std::set<std::string> _secondaryFilenames;

    FileID _nextFileID;

private:
    FileID getFileID(PageID id) {
        return static_cast<uint32_t>(id >> 16);
    }
    FileID generateFileID() {
        if (_nextFileID < MAX_FILEID - 1) {
            _nextFileID++;
        }
        return _nextFileID;
    }
    PageID generatePageID(DbFilePtr file) {
        PageID id = (file->id << 16) + _pageSize;
        assert(_maxDbFileSize - _pageSize > id);
        return id;
    }
    bool validate(PageID id, DbFilePtr file) {
        return getOffset(file, id) < file->size;
    }
    uint32_t getOffset(DbFilePtr file, PageID id) {
        return (id & 0x0000FFFF) << _pageSize;
    }

private:
    DECLARE_LOGGER(c);
};

}

#endif
