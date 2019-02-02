/*************************************************************************
    > File Name: folderdiff.h
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Thu 31 Jan 2019 10:01:15 PM CST
 ************************************************************************/
#ifndef WATCHER_FOLDERDIFF_H
#define WATCHER_FOLDERDIFF_H

#include <set>
#include <string>

namespace filewatcher {

enum PathType {
    E_FILE = 0,
    E_FOLDER,
};

struct PathInfo {
    std::string path;
    PathType type;
    bool operator < (const PathInfo & right) const {
        return path.compare(right.path) < 0;
    }
};

using PathInfoSet = std::set<PathInfo>;

class FolderDiff {
public:
    explicit FolderDiff(const std::string& path);
    void diff(PathInfoSet & add, PathInfoSet & remove);
private:
    void scan_(const std::string& path, PathType type, PathInfoSet& path_infos);
private:
    std::string folder_path_;
    PathInfoSet path_infos_;
};

}

#endif // WATCHER_FOLDERDIFF_H
