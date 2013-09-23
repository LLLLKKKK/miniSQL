#ifndef PAGE_HPP
#define PAGE_HPP

#include <memory>
#include <functional>

namespace miniSQL {

#define MAX_FILEID 0xFFFF
#define MAX_PAGEID 0xFFFFFFFF
#define INVALID_PAGEID  0x00000000
#define PAGE_SIZE  8 * 1024 * 1024


// bits [31..16] : FileID
// bits [15..00] : PageOffset
typedef uint32_t PageID;

// bits [15..8] : FileMeta
// bits [7..0]  : FileID ( Filename)
typedef uint16_t FileID;


struct Page;
typedef std::shared_ptr<Page> PagePtr;

struct Page {
    void* data;
    PageID id;
    std::function<void(Page*)> destroyCallback;
    
    ~Page() {
        if (destroyCallback) {
            destroyCallback(this);
        }
    }
};

struct PageGetSizeCallback {
    uint64_t operator() (PagePtr page) {
        return PAGE_SIZE;
    }
};

struct DbFile;
typedef std::shared_ptr<DbFile> DbFilePtr;

struct DbFile {
    FileID id;
    uint32_t size;
    uint32_t pageSize;
    int fd;
    PagePtr headerPage;
    std::string filename;

    PageID generatePageID() {
        PageID id = (this->id << 16) + pageSize;
        return id;
    }
    bool validate(PageID id) {
        return getOffset(id) < size;
    }
    uint32_t getOffset(PageID id) {
        assert(getFileID(id) == this->id);
        return (id & 0x0000FFFF) << pageSize;
    }

    static FileID _nextFileID;
    static FileID generateFileID() {
        if (_nextFileID < MAX_FILEID - 1) {
            _nextFileID++;
        }
        return _nextFileID;
    }
    static FileID getFileID(PageID id) {
        return static_cast<uint32_t>(id >> 16);
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

}

#endif
