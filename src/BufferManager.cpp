
#include "BufferManager.hpp"
#include "Logger.hpp"
#include "FixedSizeChunkAllocator.hpp"
#include <cstring>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

namespace miniSQL {

BufferManager::BufferManager(LRUCache<PageID, PagePtr, PageGetSizeCallback>* cache, 
                             FixedSizeChunkAllocator* allocator,
                             const std::string& primaryDataFile) 
    : _cache(cache), _allocator(allocator), _primaryDataFile(primaryDataFile) { }


// For serialize and deserizlize primary data file
// well, just to make it easy
// 1. 1st line: number of files
// 2. then each line for a single file

bool BufferManager::serializePrimaryDataFile() {
    // seailize them
    std::ofstream ofs;
    ofs.open(_primaryDataFile.c_str(), std::ifstream::out);
    if (! ofs.good() ) {
        MINISQL_LOG_ERROR( "Open file %s for writing failed!", _primaryDataFile.c_str());
        return false;
    }
    ofs << (int) _fileMap.size();
    for (auto& file : _fileMap) {
        ofs << file.second->filename;
    }

    return true;
}

bool BufferManager::deserializePrimaryDataFileAndLoad() {
    std::ifstream ifs;
    ifs.open(_primaryDataFile.c_str(), std::ifstream::in);
    if (! ifs.good() ) {
        MINISQL_LOG_ERROR( "Open file %s for reading failed!", _primaryDataFile.c_str());
        return false;
    }
    // open Dbfile, read header page
    int files;
    ifs >> files;
    while (files--) {
        std::string filename;
        ifs >> filename;
        DbFilePtr file = loadDbFile(filename);
        _fileMap.emplace(file->id, file);
    }

    return true;
}

DbFilePtr BufferManager::createDbFile(const std::string& filename) {
    if (ENOENT != access(filename.c_str(), O_RDWR) ) {
        MINISQL_LOG_ERROR( "Test access file %s failed!", filename.c_str());
        return nullptr;
    }
    DbFilePtr file = loadDbFile(filename);
    if ( ! file) {
        MINISQL_LOG_ERROR( "Create Db file %s fiailed!", filename.c_str());
        return nullptr;
    }
    initDbFileHeaderPage(file->headerPage);

    return file;
}


void BufferManager::initDbFileHeaderPage(PagePtr page) {
    SecondaryFileHeaderPage* header = reinterpret_cast<SecondaryFileHeaderPage*>
                                      (page.get());
    strcpy(header->header_string, "MiniSQL DbFile");
    strcpy(header->meta_version, "LK 0.0.1");
    header->page_start = 1;
    header->page_end = 1;
    header->page_slot_array_start = reinterpret_cast<PageID*>(
            reinterpret_cast<char*>(header) + PAGE_SIZE);
    header->page_slot_array_end = header->page_slot_array_end;
    
}

void BufferManager::initNormalPage(PagePtr page) {
    PageHeader* header = reinterpret_cast<PageHeader*>(page.get());
    strcpy(header->header_string, "MiniSQL Header");
    strcpy(header->meta_version, "LK 0.0.1");    
    header->page_size = PAGE_SIZE;
    header->my_id = page->id;
    header->prev_id = INVALID_PAGEID;
    header->next_id = INVALID_PAGEID;
    header->type;
    header->slot_count = 0;
    header->free_count = 0;
    header->reserved_count = 0;
    header->record_start = reinterpret_cast<char*>(header) + sizeof(header);
    header->record_end = reinterpret_cast<char*>(header) + PAGE_SIZE;
    header->record_slot_array_start = header->record_end;
    header->record_slot_array_end = header->record_start;
}

DbFilePtr BufferManager::loadDbFile(const std::string& filename) {
    int fd = open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        MINISQL_LOG_ERROR( "Can't open file %s!", filename.c_str());
        return nullptr;
    }
    DbFilePtr file(new DbFile);
    file->fd = fd;
    file->filename = filename;
    file->headerPage = getPage(PageID(0));
    SecondaryFileHeaderPage* header = reinterpret_cast<SecondaryFileHeaderPage*>
                                      (file->headerPage.get());
    file->size = header->file_size;
    file->id = header->my_id;

    return file;
}


bool BufferManager::init() {
    if ( ! deserializePrimaryDataFileAndLoad()) {
        MINISQL_LOG_ERROR( "Deserialize primary data file %s failed!",
            _primaryDataFile.c_str());
        return false;
    }

    if (_fileMap.size() == 0) {
        FileID fileID = generateFileID();
        DbFilePtr file = createDbFile(std::to_string(fileID));
        if ( ! file) {
            MINISQL_LOG_ERROR( "Create DbFile %u failed!", fileID);
            return false;
        }
        _fileMap.emplace(fileID, file);
    }
    return true;
}

BufferManager::~BufferManager() {
    serializePrimaryDataFile();
    // for (auto& page : _pageMap) {
    //     _cache.invalidate(page->first);
    // }
}

// try to allocate it in the same file
PageID BufferManager::createPage(PageID pageID) {
    FileID fileID = getFileID(pageID);
    auto it = _fileMap.find(fileID);

    if (it == _fileMap.end()) {
        MINISQL_LOG_ERROR( "");
        return INVALID_PAGEID;
    }

    auto file = it->second;
    if (_maxDbFileSize - _pageSize > file->size) {
        void* data = _allocator->allocate(_pageSize);
        uint32_t prevSize = file->size;
        void* ret =  ::mmap(data, _pageSize, PROT_READ | PROT_WRITE, 
                            MAP_PRIVATE, file->fd, prevSize);
        if (ret == MAP_FAILED) {
            MINISQL_LOG_ERROR( "can't mmap file %s !", file->filename.c_str());
            _allocator->free(data);
            return INVALID_PAGEID;
        }
        PageID id = generatePageID(file);
        PagePtr page(new Page {data, pageID, [this](Page* page) {
                        this->unmapPage(page);
                    }});        
        file->size += _pageSize;

//        _pageMap.emplace(id, page);
        _cache->put(id, page);

        return id;
    }
    // create a new file for that page
    else {
        assert(false);
        return INVALID_PAGEID;
    }
}

PagePtr BufferManager::getPage(PageID pageID) {
    PagePtr page;
    if (_cache->get(pageID, page)) {
        return page;
    }

    page = mapPage(pageID);
    if ( ! page) {
        MINISQL_LOG_ERROR( "Map page %u failed!", pageID);
        return nullptr;
    }
    _cache->put(pageID, page);

    return page;
}


PagePtr BufferManager::mapPage(PageID pageID) {
    FileID fileID = getFileID(pageID);

    auto it = _fileMap.find(fileID);
    if (it == _fileMap.end()) {
        MINISQL_LOG_ERROR( "Invalid pageID %u, can't find file!", pageID);
        return nullptr;
    }

    DbFilePtr file = it->second;
    if (! validate(pageID, file)) {
        MINISQL_LOG_ERROR( "Invalid pageID %u, can't find page!", pageID);
        return nullptr;
    }

    void* data = _allocator->allocate(_pageSize);
    void* ret =  ::mmap(data, _pageSize, PROT_READ | PROT_WRITE, 
                        MAP_PRIVATE, file->fd, getOffset(file, pageID));

    if (ret == MAP_FAILED) {
        MINISQL_LOG_ERROR( "can't mmap file %s !", file->filename.c_str());
        _allocator->free(data);
        return INVALID_PAGEID;
    }

    return PagePtr(new Page {data, pageID, [this](Page* page) {
                    this->unmapPage(page);
                }});
}

bool BufferManager::unmapPage(Page* page) {
    if (::munmap(page->data, _pageSize) < 0) {
        MINISQL_LOG_ERROR( "munmap failed for page %u", page->id);
        return false;
    }
    _allocator->free(page->data);
    return true;
}

bool BufferManager::deletePage(PageID pageID) {
    FileID fileID = getFileID(pageID);
    auto it = _fileMap.find(fileID);
    if (it == _fileMap.end()) {
        MINISQL_LOG_ERROR( "Invalid pageID %u, can't find file!", pageID);
        return false;
    }

    DbFilePtr file = it->second;
    if (! validate(pageID, file)) {
        MINISQL_LOG_ERROR( "Invalid pageID %u, can't find page!", pageID);
        return false;
    }
    
    PagePtr headerPage = file->headerPage;
    SecondaryFileHeaderPage *header = reinterpret_cast<SecondaryFileHeaderPage*>
                                      (headerPage->data);
    if (pageID != header->page_end) {
        assert(header->page_end > header->page_start);
        header->page_end--;
    }
    else {
        header->page_slot_array_start--;
        *(header->page_slot_array_start) = pageID;
    }
    _cache->invalidate(pageID);
        
    return true;
}

}
