
#ifndef _DBFILE_H_
#define _DBFILE_H_

#include <errno.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include "logger/Logger.hpp"
#include "Page.hpp"
#include "FixedSizeChunkAllocator.hpp"

namespace miniSQL {

struct Page;
struct DbFile;
typedef std::shared_ptr<DbFile> DbFilePtr;


struct DbFile {
    int fd;
    uint32_t size;
    PagePtr headerPage;
    std::string filename;
    FixedSizeChunkAllocator* allocator;
    uint32_t pageSize;
    
private:
    DbFile(int _fd, const std::string& name, 
           FixedSizeChunkAllocator* alloc, uint32_t _pageSize) 
        : fd(_fd), filename(name), allocator(alloc), pageSize(_pageSize)  { }    

    DbFile() {
        ::close(fd);
    }

public:
    static DbFilePtr open(const std::string filename, 
                          FixedSizeChunkAllocator* alloc, uint32_t _pageSize) {
        int fd = ::open(filename.c_str(), O_CREAT | O_DIRECT | O_RDWR, 
                        S_IRUSR | S_IWUSR);
        if (fd < 0) {
            return nullptr;
        }
        return DbFilePtr(new DbFile(fd, filename, alloc, _pageSize)); 
    }

    uint32_t getOffset(PageID pageID) {
        return pageSize * pageID;
    }

    bool validatePageID(PageID pageID) {
        auto header = getHeader();
        return pageSize * (1 + pageID) <= header->file_size;
    }

    SecondaryFileHeaderPage* getHeader() {
        return reinterpret_cast<SecondaryFileHeaderPage*>
            (headerPage->data);
    }

    PagePtr createHeader() {
        void* data = allocator->allocate(pageSize);
        if (ftruncate(fd, pageSize)) {
            MINISQL_LOG_ERROR( "increase file size %s failed! %s", 
                    filename.c_str(), strerror(errno));
            allocator->free(data);
            return nullptr;
        }
        size = pageSize;
        return PagePtr(new Page { data, PageID(0), true,
                        [this] (Page* page)  { this->writebackPage(page); } });
    }

    PagePtr createPage(PageID pageID) {
        void* data = allocator->allocate(pageSize);
        auto header = getHeader();
        if (ftruncate(fd, header->file_size + pageSize)) {
            MINISQL_LOG_ERROR( "increase file size %s failed! %s", 
                    filename.c_str(), strerror(errno));
            allocator->free(data);
            return nullptr;
        }
        header->file_size += pageSize;
        size = header->file_size;
        return PagePtr(new Page { data, pageID, true,
                        [this] (Page* page)  { this->writebackPage(page); } });
    }
    
    PagePtr readPage(PageID pageID) {
        void* data = allocator->allocate(pageSize);
        if (::read(fd, data, pageSize) < 0) {
            MINISQL_LOG_ERROR( "read page failed for file %s page %u, %s", 
                    filename.c_str(), pageID, strerror(errno));
            return nullptr;            
        }
        return PagePtr(new Page { data, pageID, false,
                        [this] (Page* page)  { this->writebackPage(page); } });
    }
    
    bool writebackPage(Page* page) {
        if (!page->isDirty) {
            return true;
        }
        if (::write(fd, page->data, pageSize) < 0) {
            MINISQL_LOG_ERROR( "write failed for file %s page %u, %s",
                    filename.c_str(), page->id, strerror(errno));
            return false;
        }
        page->isDirty = false;
        return true;
    }

private:
    DECLARE_LOGGER(DbFile);
};

}

#endif
