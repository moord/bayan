#include "file_utility.h"

namespace fs = std::filesystem;

//---------------------------------------------------------------------------

FileEntry::FileEntry(FileEntry && other){
    path = std::move(other.path);
    file = std::move(other.file);
    other.file = {};
    other.path = "";
};
//---------------------------------------------------------------------------

FileEntry& FileEntry::operator=(FileEntry && other){
    if(this != &other){
        path = std::move(other.path);
        file = std::move(other.file);
        other.file = {};
        other.path = "";
    }
    return *this;
}
//---------------------------------------------------------------------------

FileEntry::~FileEntry(){
    if(file && (*file)->is_open()){
        (*file)->close();
        delete (*file);
    }
    file = {};
};
//---------------------------------------------------------------------------

std::string FileEntry::GetChunkHash(unsigned int nToRead, unsigned int blockSize, HashFunc &H){
    unsigned char *data = new unsigned char[blockSize];
    if( !file){
        file = new std::ifstream(path.data(), std::ios::binary);
    }

    (*file)->read((char*)(data), nToRead);

    memset( (data+nToRead), 0, blockSize-nToRead);

    std::string hash = H(data, blockSize);

    delete [] data;
    return hash;
}
//---------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const FileEntry & fe){
    os << fe.path;
    return os;
}
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// FileComparator
/////////////////////////////////////////////////////////////////////////////

void FileComparator::CompareChunks( std::list<FileEntry> &v, unsigned int left ){
    FileGroups<std::string> groups;    // map - в качестве ключа хэш сегмента

    int nToRead = (left >= blockSize) ? blockSize : left;


    // группируем файлы с одинаковыми хэшами для прочитанных сегментов
    for (auto &f: v) {
        groups[f.GetChunkHash(nToRead,blockSize, H)].emplace_back(std::move(f));
    }

    left -= nToRead;

    for (auto &[hash, group]: groups) {
        if( group.size() > 1){                        // если в группе несколько файлов с одинаковыми хэшами для прочитанных сегментов
            if( left == 0){                           // если все сегменты считаны, то выводим результат
                std::copy(group.begin(), group.end(), std::ostream_iterator<FileEntry>(std::cout,"\n"));
                std::cout << std::endl;
            }else{
                CompareChunks( group, left);    // иначе считываем  и сравниваем  следущие сегменты
            }
        }
    }
}
//---------------------------------------------------------------------------

void FileComparator::Execute(){
    FileGroups<unsigned int> groups;   // map - в качестве ключа размер файла

    try
    {
        // группируем файлы одинакового размера
        for( const auto &dir : paths){
            if(!ignorePaths.count(dir)){
                for (auto i = fs::recursive_directory_iterator(dir); i != fs::recursive_directory_iterator(); ++i)
                {
                    if( i->is_directory() && (i.depth() >= depth || ignorePaths.count(i->path()))) {
                        i.disable_recursion_pending();
                    }
                    else if (i->is_regular_file()){

                        if( i->file_size() >= minFileSize && (!mask || boost::regex_match(i->path().filename().c_str(), *mask))){
                            groups[i->file_size()].emplace_back(i->path());
                        }
                    }
                }
            }
        }
    }
    catch(const fs::filesystem_error& e)
    {
        std::cout << e.what() << '\n';
    }

     for (auto &[size, group]: groups) {
        if( group.size() > 1){              // если в группе несколько файлов одного размера
            CompareChunks( group, size);    // то сравниваем первые сегменты
        }
    }

}
//---------------------------------------------------------------------------

