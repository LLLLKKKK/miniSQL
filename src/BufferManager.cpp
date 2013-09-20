
#include "BufferManager.hpp"
#include <cstring>
#include <fcntl.h>
#include <fstream>

BufferManager::BufferManager(LRUCache<PageID, PagePtr>* cache, 
                             FixedSizeChunkAllocator* allocator,
                             const std::string& primaryDataFile) 
    : _cache(cache), _allocator(allocator), _filename(filename) { }


// For serialize and deserizlize primary data file
// well, just to make it easy
// 1. 1st line: number of files
// 2. then each line for a single file

bool BufferManager::serializePrimaryDataFile() {
    // seailize them
    std::ofstream ofs;
    ofs.open(_primaryDataFile.c_str(), std::ifstream::out);
    if (! ifs.good() ) {
        LOG_ERROR("Open file %s for writing failed!", _primaryDataFile.c_str());
        return false;
    }
    ofs << (int) _fileMap.size();
    for (auto it = _fileMap.begin(); it != _fileMap.end(); it++) {
        ofs << it->second->filename;
    }

    return true;
}

bool BufferManager::deserializePrimaryDataFileAndLoad() {
    std::ifstream ifs;
    ifs.open(_primaryDataFile.c_str(), std::ifstream::in);
    if (! ifs.good() ) {
        LOG_ERROR("Open file %s for reading failed!", _primaryDataFile.c_str());
        return false;
    }
    // open Dbfile, read header page
    int files;
    ifs >> files;
    while (files--) {
        std::string filename;
        ifs >> filename;
        DbFilePtr file = loadDbFile(filename);
        _fileMap.emplace(file->fileID, file);
    }

    return true;
}

DbFilePtr BufferManager::createDbFile(const std::string& filename) {
    if (ENOENT != access(filename.c_str(), O_RDWR) ) {
        LOG_ERROR("Test access file %s failed!", filename.c_str());
        return nullptr;
    }
    DbFilePtr file = loadDbFile(filename);
    if ( ! file) {
        LOG_ERROR("Create Db file %s fiailed!", filename.c_str());
        return nullptr;
    }
    initDbFileHeaderPage(file->header);

    return file;
}


void initDbFileHeaderPage(PagePtr page) {
    SecondaryFileHeaderPage* header = reinterpret_cast<SecondaryFileHeaderPage*>
                                      (page->get());
    strcpy(header->header_string, "MiniSQL DbFile");
    strcpy(header->meta_version, "LK 0.0.1");
    header->page_start = 1;
    header->page_end
    
}

void initNormalPage(PagePtr page) {
    PageHeader* header = reinterpret_cast<SecondaryFileHeaderPage*>
                         (page->get());
    strcpy(header->header_string, "MiniSQL Header");
    strcpy(header->meta_version, "LK 0.0.1");
    
}

DbFilePtr BufferManager::loadDbFile(const std::string& filename) {
    int fd = open(filename.c_str(), O_CREAT | O_RDWR);
    if (fd < 0) {
        LOG_ERROR("Can't open file %s!", filename.c_str());
        return nullptr;
    }
    DbFilePtr file(new DbFile);
    file->fd = fd;
    file->filename = filename;
    file->header = getPage(PageID(0));
    SecondaryFileHeaderPage* header = reinterpret_cast<SecondaryFileHeaderPage*>
                                      (file->header->get());
    file->size = header->file_size;
    file->id = header->my_id;

    return file;
}


bool BufferManager::init() {
    if (!deserializePrimaryDataFileAndLoad()) {
        LOG_ERROR("Deserialize primary data file %s failed!",
                  _primaryDataFile.c_str());
        return false;
    }

    if (_fileMap.size() == 0) {
        FileID fileID = generateFileID();
        DbFilePtr file = createDbFile(fileID);
        if ( ! file) {
            LOG_ERROR("Create DbFile %u failed!", fileID);
            return false;
        }
        _fileMap.emplace(FileID, file);
    }
    return true;
}

BufferManager::~BufferManager() {
    serializePrimaryDataFile();
    for (auto it = _pageMap.begin(); it != _pageMap.end(); it++) {
        _cache.invalidate(it->first);
    }
}

// try to allocate it in the same file
PageID BufferManager::createPage(PageID pageID) {
    FileID fileID = getFileID(pageID);
    auto it = _nonfullFileMap.find(fileID);

    if (it == _nonfullFileMap.end()) {
        LOG_ERROR("");
        return Page::INVALIDATE_PAGEID;
    }

    DbFile* file = it->second;
    if (maxDbFileSize - pageSize > file->size) {
        void* data = _alllocator->allocate(_pageSize);
        uint32_t prevSize = file->size;
        void* ret =  ::mmap(data, _pageSize, PROT_READ | PROT_WRITE, 
                            MAP_PRIVATE, file->fd, prevSize);
        if (ret == MAP_INVALLID) {
            LOG_ERROR("can't mmap file %s !", file->filename.c_str());
            _allocator->release(data);
            return Page::INVALIDATE_PAGEID;
        }
        PageID id = generatePageID(file);
        PagePtr page = PagePtr(new Page(data, id, this));
        file->size += pageSize;

        _pageMap.emplace(id, page);
        _cache->put(id, page);

        return id;
    }
    // create a new file for that page
    else {
        assert(false);
        return Page::INVALIDATE_PAGEID;
    }
}

PagePtr BufferManager::getPage(PageID pageID) {
    PagePtr page;
    if (_cache->get(pageID, page)) {
        return page;
    }

    PagePtr page = mapPage(pageID);
    if ( ! page) {
        LOG_ERROR("Map page %u failed!", pageID);
        return nullptr;
    }
    _cache->put(pageID, page);

    return page;
}


PagePtr mapPage(PageID pageID) {
    FileID fileID = getFileID(pageID);

    auto it = _fileMap.find(fileID);
    if (it == _fileMap.end()) {
        LOG_ERROR("Invalid pageID %u, can't find file!", pageID);
        return nullptr;
    }

    DbFile* file = it->second;
    if (! validate(pageID, file)) {
        LOG_ERROR("Invalid pageID %u, can't find page!", pageID);
        return nullptr;
    }

    void* data = _alllocator->allocate(_pageSize);
    void* ret =  ::mmap(data, _pageSize, PROT_READ | PROT_WRITE, 
                        MAP_PRIVATE, file->fd, getOffset(file, pageID));

    if (ret == MAP_INVALLID) {
        LOG_ERROR("can't mmap file %s !", file->filename.c_str());
        _allocator->release(data);
        return Page::INVALIDATE_PAGEID;
    }

    return PagePtr(new Page(data, pageID, this));
}

bool unmapPage(PagePtr page) {
    if (::munmap(page->data, _pageSize) < 0) {
        LOG_ERROR("munmap failed for page %u", page->id);
        return false;
    }
    _allocator->release(data);
    return true;
}

bool BufferManager::deletePage(PageID pageID) {
    FileID fileID = getFileID(pageID);
    auto it = _fileMap.find(fileID);
    if (it == _fileMap.end()) {
        LOG_ERROR("Invalid pageID %u, can't find file!", pageID);
        return false;
    }

    DbFile* file = it->second;
    if (! validate(pageID, file)) {
        LOG_ERROR("Invalid pageID %u, can't find page!", pageID);
        return false;
    }
    
    PagePtr headerPage = file->headerPage->get();
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

