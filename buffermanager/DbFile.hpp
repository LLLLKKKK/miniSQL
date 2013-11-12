
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
class DbFile;
typedef std::shared_ptr<DbFile> DbFilePtr;


class DbFile {
public:
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
                          FixedSizeChunkAllocator* alloc, uint32_t _pageSize);

public:
    uint32_t getOffset(PageID pageID);
    bool validatePageID(PageID pageID);
    FileHeaderPage* getHeader();
    PagePtr createHeader();
    PagePtr createPage(PageID pageID);
    PagePtr readPage(PageID pageID);
    bool writebackPage(Page* page);


private:
    DECLARE_LOGGER(DbFile);
};

}

#endif
