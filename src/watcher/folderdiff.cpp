/*************************************************************************
    > File Name: folderdiff.cpp
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Thu 31 Jan 2019 10:01:21 PM CST
 ************************************************************************/
#include "folderdiff.h"
#include "utils/file_util.h"
#include "utils/folderscan.h"

#include <iostream>
#include <algorithm>

namespace filewatcher {

FolderDiff::FolderDiff(const std::string& path){
    folder_path_ = absolute(path);

    PathInfoSet path_infos;
    folder_scan(folder_path_, 
        std::bind(&FolderDiff::scan_, this, std::placeholders::_1, E_FILE, std::ref(path_infos_)),
        std::bind(&FolderDiff::scan_, this, std::placeholders::_1, E_FOLDER, std::ref(path_infos_)));
}

void FolderDiff::scan_(const std::string& path, PathType type, PathInfoSet& path_infos) {
    PathInfo pi{path, type};
    path_infos.insert(pi);
}

void FolderDiff::diff(PathInfoSet & add, PathInfoSet & remove) {
    PathInfoSet path_infos;
    folder_scan(folder_path_, 
        std::bind(&FolderDiff::scan_, this, std::placeholders::_1, E_FILE, std::ref(path_infos)),
        std::bind(&FolderDiff::scan_, this, std::placeholders::_1, E_FOLDER, std::ref(path_infos)));

    std::set_difference(path_infos.begin(), path_infos.end(),
                        path_infos_.begin(), path_infos_.end(), std::inserter(add, add.begin()));

    std::set_difference(path_infos_.begin(), path_infos_.end(),
                    path_infos.begin(), path_infos.end(), std::inserter(remove, remove.begin()));
    path_infos_ = path_infos;
}


}
