#include <iostream>

#include <boost/program_options.hpp>
#include "file_utility.h"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    std::vector<std::string> paths;
    paths.emplace_back("/home/andrew/projects/bayan/test/");

    FileComparator fc;
 
    std::string input;
    std::string output;

    po::options_description desc ("Доступные опции");
    desc.add_options ()
            ("help,h", "описание доступных опций")
            ("hash,H", po::value<std::string>(), "алгоритм хэширования")
            ("block-size,S", po::value<int>(), "размер блока")
            ("mask,m", po::value<std::string>(), "маски имен файлов разрешенных для сравнения")
            ("size,s", po::value<int>(), "минимальный размер файла")
            ("depth,d", po::value<int>(), "уровень сканирования")
            ("exclude-path,E", po::value<std::vector<std::string> >(), "директории для сканирования (может быть несколько)")
            ("include-path,I", po::value<std::vector<std::string> >(), "директории для исключения из сканирования");

    po::variables_map vm;
    po::store (po::command_line_parser (argc, argv).options (desc).run (), vm);
    po::notify (vm);

    if (vm.count ("help") ) {
        std::cerr << desc << "\n";
        return 1;
    }
    if (vm.count ("include-path")){
        fc.SetPaths(vm["include-path"].as< std::vector<std::string> >());
    }

    if (vm.count ("exclude-path") ) {
        fc.SetIgnorePaths(vm["exclude-path"].as< std::vector<std::string> >());
    }

    if (vm.count ("depth") ) {
        fc.SetDepth(vm["depth"].as<int>());
    }

    if (vm.count ("size") ) {
        fc.SetMinFileSize(vm["size"].as<int>());
    }

    if (vm.count ("mask") ) {
        fc.SetMask(vm["mask"].as<std::string>());

    }

    if (vm.count ("block-size") ) {
        fc.SetBlockSize(vm["block-size"].as<int>());
    }

    if (vm.count ("hash") ) {
        fc.SetHashFunc(vm["hash"].as<std::string>());
    }

    fc.Execute();

    return 0;
}
