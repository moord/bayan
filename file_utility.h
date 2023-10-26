#ifndef FILE_UTILITY_H
#define FILE_UTILITY_H

#include <boost/regex.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <functional>
#include <filesystem>
#include <optional>
#include <list>

#include "lib/md5.h"
#include "lib/sha256.h"
#include "lib/crc32.h"

#include "helper.h"

using HashFunc = std::function < std::string(const unsigned char data[], unsigned int size) > ;

class FileEntry
{
private:
    std::optional<std::ifstream*> file;
    std::string path;
public:
    explicit FileEntry(std::string APath) :path( APath ){};
    FileEntry(const FileEntry &) = delete;
    FileEntry& operator=(const FileEntry &) = delete;
    FileEntry(FileEntry && other);
    FileEntry& operator=(FileEntry && other);
    ~FileEntry();

    std::string GetChunkHash(unsigned int nToRead, unsigned int blockSize, HashFunc &H);
    friend std::ostream& operator<<(std::ostream& os, const FileEntry & fe);

};

template<typename T>
using FileGroups = std::map<T, std::list<FileEntry>>;

class FileComparator{
private:
    HashFunc H;
    unsigned int blockSize,
    minFileSize,
    depth;
    std::optional<boost::regex> mask;
    std::set<std::string>ignorePaths;
    std::vector<std::string>paths;


    void CompareChunks( std::list<FileEntry> &v, unsigned int left);

public:
    FileComparator():blockSize(10),minFileSize(0), depth(0), H(crc32){};

    void SetBlockSize(int ABlockSize){
        blockSize = ABlockSize;
    }

    void SetMinFileSize(int AMinFileSize){
        minFileSize = AMinFileSize;
    }

    void SetDepth(int ADepth){
        depth = ADepth;
    }

    void SetMask(std::string AWildcard){
        if(AWildcard.length()){
            mask = boost::regex(Helper::WildcardToRegex(AWildcard), boost::regex::icase);
        } else {
            mask = {};
        }
    }

    void SetHashFunc(std::string AHashName){
        if(AHashName == "md5" || AHashName == "MD5"){
            H = md5;
        } else if(AHashName == "sha256" || AHashName == "SHA256"){
            H = sha256;
        } else{
            H = crc32;
        }

    };

    void SetPaths(const std::vector<std::string> &APaths ){
        paths = APaths;
    };

    void SetIgnorePaths(const std::vector<std::string> &APaths ){
        std::copy(APaths.begin(), APaths.end(), std::inserter(ignorePaths, ignorePaths.end()));
    };

    void Execute();
};

#endif // FILE_UTILITY_H
