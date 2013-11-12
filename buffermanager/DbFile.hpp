
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

struct DbFile;
typedef std::shared_ptr<DbFile> DbFilePtr;


struct DbFile {
    int fd;
    uint32_t size;
    uint32_t pageSize;
    PagePtr headerPage;
    std::string filename;
    FixedSizeChunkAllocator* allocator;
    
private:
    DbFile(int _fd, const std::string& name, FixedSizeChunkAllocator* alloc) 
        : fd(_fd), filename(name), allocator(alloc)  { }    
public:
    static DbFilePtr open(const std::string filename, FixedSizeChunkAllocator* alloc) {
        int fd = ::open(filename.c_str(), O_CREAT | O_DIRECT | O_RDWR, 
                        S_IRUSR | S_IWUSR);
        if (fd < 0) {
            MINISQL_LOG_ERROR( "Can't open file %s!", filename.c_str());
            return nullptr;
        }
        return DbFilePtr(new DbFile(fd, filename, alloc)); 
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
            (headerPage.get());
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
        return PagePtr(new Page { data, pageID });  
    }

    PagePtr readPage(DbFilePtr file, PageID pageID) {
        void* data = allocator->allocate(pageSize);
        if (::read(file->fd, data, pageSize) < 0) {
            MINISQL_LOG_ERROR( "read page failed for file %s page %u, %s", 
                    filename.c_str(), pageID, strerror(errno));
            return nullptr;            
        }
        return PagePtr(new Page { data, pageID });
    }
    
    bool writebackPage(PagePtr page) {
        if (::write(fd, page->data, pageSize) < 0) {
            MINISQL_LOG_ERROR( "write failed for file %s page %u, %s",
                    filename.c_str(), page->id, strerror(errno));
            return false;
        }
        return true;
    }

private:
    DECLARE_LOGGER(DbFile);
};

}

#endif
