#ifndef PAGE_HPP
#define PAGE_HPP

#include <memory>
#include <functional>

namespace miniSQL {

#define MAX_FILEID 0xFFFF
#define MAX_PAGEID 0xFFFFFFFF
#define INVALID_PAGEID 0x00000000
#define PAGE_SIZE (8 * 1024)
#define BUFFER_SIZE (20 * 1024 * 1024)
#define MAX_FILE_SIZE (1024 * 1024 * 1024)

#define FILE_HEADER_STRING "MiniSQL DbFile"
#define PAGE_HEADER_STRING "MiniSQL Page"
#define META_VERSION "LK 0.0.1"

typedef uint32_t PageID;

class DbFile;

struct Page {
    void* data;
    PageID id;
    bool isDirty;
    DbFile *file;

    ~Page();
};

typedef std::shared_ptr<Page> PagePtr;

struct FileHeaderPage;

struct __attribute__((packed)) FileHeaderPage {
    char header_string[16];
    char meta_version[16];

    uint32_t file_size;
    uint32_t type;

    PageID page_start;
    PageID page_end;

    uint32_t page_slot_array_start;
    uint32_t page_slot_array_end;

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
    uint32_t reserved[3];
};

}

#endif
