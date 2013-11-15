

#include "DbFile.h"

namespace miniSQL {

DbFilePtr DbFile::open(const std::string filename, 
                      FixedSizeChunkAllocator* alloc, uint32_t _pageSize) {
    int fd = ::open(filename.c_str(), O_CREAT | O_DIRECT | O_RDWR, 
                    S_IRUSR | S_IWUSR);
    if (fd < 0) {
        return nullptr;
    }
    return DbFilePtr(new DbFile(fd, filename, alloc, _pageSize)); 
}

uint32_t DbFile::getOffset(PageID pageID) {
    return pageSize * pageID;
}

bool DbFile::validatePageID(PageID pageID) {
    auto header = getHeader();
    return pageSize * (1 + pageID) <= header->file_size;
}

FileHeaderPage* DbFile::getHeader() {
    return reinterpret_cast<FileHeaderPage*>
        (headerPage->data);
}

PagePtr DbFile::createHeader() {
    void* data = allocator->allocate(pageSize);
    if (ftruncate(fd, pageSize)) {
        MINISQL_LOG_ERROR( "increase file size %s failed! %s", 
                          filename.c_str(), strerror(errno));
        allocator->free(data);
        return nullptr;
    }
    size = pageSize;
    return PagePtr(new Page { data, PageID(0), true, this});
}

PagePtr DbFile::createPage(PageID pageID) {
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
    return PagePtr(new Page { data, pageID, true, this });
}
    
PagePtr DbFile::readPage(PageID pageID) {
    void* data = allocator->allocate(pageSize);
    if (::read(fd, data, pageSize) < 0) {
        MINISQL_LOG_ERROR( "read page failed for file %s page %u, %s", 
                          filename.c_str(), pageID, strerror(errno));
        return nullptr;            
    }
    return PagePtr(new Page { data, pageID, false, this });
}
    
bool DbFile::writebackPage(Page* page) {
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

}
