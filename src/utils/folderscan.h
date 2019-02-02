/*************************************************************************
    > File Name: folderscan.h
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Thu 31 Jan 2019 10:08:27 PM CST
 ************************************************************************/
#ifndef UTILS_FOLDERSCAN_H
#define UTILS_FOLDERSCAN_H

#include <string>
#include <functional>

namespace filewatcher {

using callback = std::function<void(const std::string&)>;
void folder_scan(const std::string& path, callback file_cb, callback folder_cb);

}

#endif // UTILS_FOLDERSCAN_H
