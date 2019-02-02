/*************************************************************************
    > File Name: file_util.h
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Thu 31 Jan 2019 02:35:43 PM CST
 ************************************************************************/
#ifndef UTILS_FILE_UTIL_H
#define UTILS_FILE_UTIL_H

#include <string>

namespace filewatcher {

std::string absolute(const std::string& path);
void create_folder(const std::string path);

}

#endif // UTILS_FILE_UTIL_H
