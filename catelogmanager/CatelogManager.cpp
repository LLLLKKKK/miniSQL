
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
        MINISQL_LOG_ERROR( "open file %s for reading failed!", _primaryDataFile.c_str());
        return false;
    }
    // open Dbfile, read header page
    int files;
    ifs >> files;
    while (files--) {
        std::string filename;
        ifs >> filename;
        DbFilePtr file = loadDbFile(filename);
        if ( ! file) {
            MINISQL_LOG_ERROR( "load db file %s failed!", filename.c_str());
            return false;
        }
        _fileMap.emplace(file->id, file);
    }

    return true;
}
