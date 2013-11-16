
#include <cstring>
#include <fstream>

#include "common.h"
#include "logger/Logger.h"
#include "BufferManager.h"
#include "FixedSizeChunkAllocator.h"

namespace miniSQL { 

BufferManager::BufferManager() : 
    _maxDbFileSize(MAX_FILE_SIZE),
    _pageSize(PAGE_SIZE),
    _cache(BUFFER_SIZE)
{ }

BufferManager::~BufferManager() { 
    _cache.clear();
    _fileMap.clear();
}

bool BufferManager::init() {
    if (!_allocator.init(PAGE_SIZE, BUFFER_SIZE / PAGE_SIZE)) {
        MINISQL_LOG_ERROR("Allocator init failed!");
        return false;
    }
    return true;
}

DbFilePtr BufferManager::createDbFile(const std::string& filename) {
    return loadDbFile(filename, true);
}

DbFilePtr BufferManager::loadDbFile(const std::string& filename, bool isNew) {
    if (_fileMap.find(filename) != _fileMap.end()) {
        MINISQL_LOG_WARN( "File has been opened %s!", filename.c_str());
        return nullptr;
    }

    auto file = DbFile::open(filename, &_allocator, _pageSize);
    if (!file) {
        MINISQL_LOG_ERROR( "Can't open file %s!", filename.c_str());
        return nullptr;
    }

    PagePtr headerPage;
    if (isNew) {
        headerPage = file->createHeader();
    } else {
        headerPage = file->readPage(PageID(0));
    }

    if (!headerPage) {
        MINISQL_LOG_ERROR( "Can't map file %s!", filename.c_str());
        return nullptr;
    }

    file->headerPage = headerPage;
    
    if (isNew) {
        initDbFileHeaderPage(file->headerPage);
    }

    auto header = file->getHeader();
    file->filename = filename;
    file->pageSize = _pageSize;
    file->headerPage = headerPage;
    file->size = header->file_size;

    _cache.put(std::make_pair(filename, headerPage->id), 
               headerPage, true);
    _fileMap.insert(make_pair(filename, file));
    
    return file;
}

void BufferManager::initDbFileHeaderPage(PagePtr page) {
    FileHeaderPage* header = reinterpret_cast<FileHeaderPage*>
                                      (page->data);
    strcpy(header->header_string, FILE_HEADER_STRING);
    strcpy(header->meta_version, META_VERSION);
    header->file_size = PAGE_SIZE;
    //header->type;
    header->page_start = INVALID_PAGEID;
    header->page_end = INVALID_PAGEID;
    header->page_slot_array_end = PAGE_SIZE;
    header->page_slot_array_start = PAGE_SIZE;
}

void BufferManager::initNormalPage(PagePtr page) {
    PageHeader* header = reinterpret_cast<PageHeader*>(page->data);
    strcpy(header->header_string, PAGE_HEADER_STRING);
    strcpy(header->meta_version, META_VERSION);
    header->page_size = PAGE_SIZE;
    header->my_id = page->id;
    header->prev_id = INVALID_PAGEID;
    header->next_id = INVALID_PAGEID;
    //header->type;
}

// try to allocate it in the same file
PagePtr BufferManager::createPage(const std::string& filename) {
    auto it = _fileMap.find(filename);

    if (it == _fileMap.end()) {
        MINISQL_LOG_ERROR("invalid filename %s!", filename.c_str());
        return INVALID_PAGEID;
    }
    
    auto file = it->second;
    auto header = file->getHeader();
    
    // get page from slot
    if (header->page_slot_array_start != header->page_slot_array_end) {
        auto now_ptr = reinterpret_cast<PageID*>
                       (reinterpret_cast<char*>(header) + header->page_slot_array_start);
        auto pageID = *now_ptr;
        header->page_slot_array_start += sizeof(PageID) / sizeof(char);
        return getPage(filename, pageID);
    }
    
    // create a new page
    if (_maxDbFileSize - _pageSize > file->size) {
        
        PageID pageID = ++header->page_end;
        PagePtr page = file->createPage(pageID);
        if (!page) {
            MINISQL_LOG_ERROR("Create page %s failed!", std::to_string(pageID).c_str());
            return INVALID_PAGEID;
        }
        initNormalPage(page);

        _cache.put(make_pair(filename, pageID), page);

        return page;
    }
    else {
        MINISQL_LOG_ERROR("File [%s] exceed max size!", filename.c_str());
        return nullptr;
    }
}

PagePtr BufferManager::getPage(const std::string& filename, PageID pageID) {
    PagePtr page;
    if (_cache.get(make_pair(filename, pageID), page)) {
        return page;
    }

    auto it = _fileMap.find(filename);
    
    if (it == _fileMap.end()) {
        MINISQL_LOG_ERROR( "invalid pageID %u, can't find file!", pageID);
        return nullptr;
    }

    DbFilePtr file = it->second;
    if (! file->validatePageID(pageID)) {
        MINISQL_LOG_ERROR( "invalid pageID %u, can't find page!", pageID);
        return nullptr;
    }

    page = file->readPage(pageID);
    if ( ! page) {
        MINISQL_LOG_ERROR( "Read page %u failed!", pageID);
        return nullptr;
    }
    
    _cache.put(make_pair(filename, pageID), page);

    return page;
}


bool BufferManager::deletePage(const std::string& filename, PageID pageID) {
    if (pageID == INVALID_PAGEID) {
        MINISQL_LOG_ERROR( "invalid pageID %u!", pageID);
        return false;
    }
    
    auto it = _fileMap.find(filename);
    if (it == _fileMap.end()) {
        MINISQL_LOG_ERROR( "invalid pageID %u, can't find file!", pageID);
        return false;
    }
    
    DbFilePtr file = it->second;
    if (! file->validatePageID(pageID)) {
        MINISQL_LOG_ERROR( "invalid pageID %u, can't find page!", pageID);
        return false;
    }
    
    auto header = file->getHeader();
    if (pageID == header->page_end) {
        assert(header->page_end > header->page_start);
        header->page_end--;
        header->file_size -= _pageSize;
        if (ftruncate(file->fd, header->file_size)) {
            MINISQL_LOG_ERROR( "increase file size %s failed!", filename.c_str());
            return nullptr;
        }
    }
    else {
        auto slot_start = reinterpret_cast<PageID*>
                          ((reinterpret_cast<char*>(header) + header->page_slot_array_start));
        slot_start--;
        *slot_start = pageID;
        header->page_slot_array_start -= sizeof(PageID) / sizeof(char);
    }
    _cache.invalidate(make_pair(filename, pageID));
    
    return true;
}

bool BufferManager::validatePage(const std::string& filename, PageID pageID) {
    if (pageID == INVALID_PAGEID) {
        return false;
    }
    
    auto it = _fileMap.find(filename);
    if (it == _fileMap.end()) {
        return false;
    }
    
    DbFilePtr file = it->second;
    if (! file->validatePageID(pageID)) {
        return false;
    }

    return true;
}

}
