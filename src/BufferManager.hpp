
#ifndef BUFFERMANAGER_HPP
#define BUFFERMANAGER_HPP

#include <map>
#include <string>
#include "LRUCache.hpp"

class FixedSizeChunkAllocator;
class BufferManager;


// bits [31..16] : FileID
// bits [15..00] : PageOffset
typedef uint32_t PageID;

// bits [15..8] : FileMeta
// bits [7..0]  : FileID ( Filename)
typedef uint16_t FileID;

const uint64_t PAGE_SIZE = 8 * 1024 * 1024;
#define MAX_FILEID 0xFFFF
#define MAX_PAGEID 0xFFFFFFFF

struct Page {
    void* data;
    PageID id;
    BufferManager* manager;

    static PageID INVALID_PAGEID = 0x00000000;

    DISALLOW_CONSTRUCT(Page);
    
    Page(void* data_, PageID id_, BufferManager* manager_) 
        : data(data_), id(id_),  manager(manager_) { }
    ~Page() {
        manager->unpinPage(id);
    }
};

struct PageGetSizeCallback {
    uint64_t operator() (PagePtr page) {
        return PAGE_SIZE;
    }
};

struct __attribute__((packed)) SecondaryFileHeaderPage {
    char header_string[16];
    char meta_version[16];

    uint32_t file_size;

    FileID my_id;
    FileID padding1;
    FileID prev_id;
    FileID padding2;
    FileID next_id;
    FileID padding3;

    uint32_t type;

    PageID page_start;
    PageID page_end;

    PageID* page_slot_array_start;
    PageID* page_slot_array_end;

    uint32_t reserved[16];
};

struct __attribute__((packed)) PageHeader {
    char header_string[16];
    char meta_version[16];

    uint32_t page_size;

    PageID my_id;
    PageID prev_id;
    PageID next_id;

    uint32_t type;

    uint32_t slot_count;
    uint32_t free_count;
    uint32_t reserved_count;

    void* record_start;
    void* record_end;

    void* record_slot_array_start;
    void* record_slot_array_end;

    uint32_t reserved[16];
};


typedef std::shared_ptr<Page> PagePtr;


struct DbFile {
    FileID fileID;
    uint32_t size;
    int fd;
    PagePtr headerPage;
    std::string filename;
};

typedef std::shared_ptr<DbFile> DbFilePtr;

class BufferManager {

public:
    BufferManager(LRUCache<PageID, PagePtr, PageGetSizeCallback>* cache, 
                  FixedSizeChunkAllocator* allocator, 
                  const std::string& primaryDataFile);
    ~BufferManager();

    DISALLOW_COPY_AND_ASSGIN(BufferManager);

public:
    bool init();

    PageID createPage();
    bool pinPage(PageID PageID);
    bool unpinPage(PageID PageID);
    bool deletePage(PageID PageID);
    PagePtr getPage(PageID pageID);

private:
    DbFilePtr createDbFile();
    DbFilePtr loadDbFile(const std::string& filename);

private:
    FileID getFileID(PageID id) {
        return static_cast<uint32_t>(id >> 32);
    }
    FileID generateFileID() {
        if (_nextFileID < MAX_FILEID - 1) {
            _nexFileID++;
        }
        return _nexFileID;
    }
    PageID generatePageID(DbFile* file) {
        PageID id = file->id << 32 + size;
        assert(maxDbFileSize - pageSize > size);
        return size;
    }
    bool validate(DbFile* file, PageID id) {
        return getOffset(file, id) < file->size;
    }
    uint32_t getOffset(DbFile* file, PageID id) {
        return (id & 0x0000FFFF) << _pageSize;
    }

private:

    uint32_t _maxDbFileSize;
    uint32_t _pageSize;

    const std::string _primaryDataFile;
    LRUCache<PageID, PagePtr> *_cache;
    FixedSizeChunkAllocator *_allocator;
    std::map<FileID, DbFile*> _fileMap;
//    std::map<FileID, DbFile*> _nonfullFileMap;
    
//    std::set<std::string> _secondaryFilenames;

    FileID _nextFileID;
};

#endif
