
#ifndef BUFFERMANAGER_HPP
#define BUFFERMANAGER_HPP

#include <set>
#include <map>
#include <string>
#include <cassert>
#include "common.h"
#include "FixedSizeChunkAllocator.hpp"
#include "LRUCache.hpp"
#include "Page.hpp"
#include "DbFile.hpp"

namespace miniSQL {

class FixedSizeChunkAllocator;
class BufferManager;
class Logger;

struct PageGetSizeCallback {
    uint64_t operator() (PagePtr page) {
        return PAGE_SIZE;
    }
};


class BufferManager {

public:
    BufferManager();
    ~BufferManager();

    DISALLOW_COPY_AND_ASSIGN(BufferManager);

public:
    bool init();

    PageID createPage(const std::string& filename);
    bool pinPage(const std::string& filename, PageID PageID);
    bool unpinPage(const std::string& filename, PageID PageID);
    bool deletePage(const std::string& filename, PageID PageID);
    PagePtr getPage(const std::string& filename, PageID pageID);

    DbFilePtr loadDbFile(const std::string& filename, bool isNew = false);
    DbFilePtr createDbFile(const std::string& filename);

private:
    void initDbFileHeaderPage(PagePtr);
    void initNormalPage(PagePtr);

private:
    uint32_t _maxDbFileSize;
    uint32_t _pageSize;

    LRUCache<std::pair<std::string, PageID>, PagePtr, PageGetSizeCallback> _cache;
    FixedSizeChunkAllocator _allocator;

    std::map<std::string, DbFilePtr> _fileMap;

private:
    DECLARE_LOGGER(BufferManager);
};

}

#endif
