/*************************************************************************
    > File Name: file_util.cpp
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Thu 31 Jan 2019 02:35:51 PM CST
 ************************************************************************/
#include "file_util.h"

#include <boost/filesystem.hpp>

namespace filewatcher {

std::string absolute(const std::string& path) {
    if (boost::filesystem::path(path).is_absolute()) {
        return path;
    }
    std::string absolute_path = boost::filesystem::system_complete(path).string();
    return absolute_path;
}

void create_folder(const std::string path) {
    if (!boost::filesystem::is_directory(path)) {
            boost::filesystem::remove(path);
    }
    if (!boost::filesystem::exists(path)) {
        boost::filesystem::create_directory(path);
    }
}

}
