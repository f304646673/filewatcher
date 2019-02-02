/*************************************************************************
    > File Name: folderscan.cpp
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Thu 31 Jan 2019 10:08:31 PM CST
 ************************************************************************/
#include "folderscan.h"

#include <boost/filesystem.hpp>

namespace filewatcher {

void folder_scan(const std::string& path, callback file_cb, callback folder_cb) {
    if (!boost::filesystem::is_directory(path)) {
        return;
    }
    
    if (!boost::filesystem::exists(path)) {
        return;
    }

    boost::filesystem::directory_iterator it(path);
    boost::filesystem::directory_iterator end;
    for (; it != end; it++) {
        if (boost::filesystem::is_directory(*it)) {
            folder_cb(it->path().string());
            folder_scan(it->path().string(), file_cb, folder_cb);
        }
        else {
            file_cb(it->path().string());
        }
    }
}

}
