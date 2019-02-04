/*************************************************************************
    > File Name: filewatcher.cpp
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Wed 30 Jan 2019 03:07:08 PM CST
 ************************************************************************/
#include "filewatcher.h"
#include "utils/file_util.h"

#include <boost/filesystem.hpp>

namespace filewatcher {

FileWatcher::~FileWatcher() {
}

FileWatcher::FileWatcher(const std::string& path, callback cb, LibevLoop* loop) {
    file_path_ = absolute(path);
    cb_ = std::move(cb);

    if (boost::filesystem::is_directory(file_path_)) {
        return;
    }

    if (boost::filesystem::is_regular_file(file_path_)) {
        last_write_time_ = boost::filesystem::last_write_time(file_path_);
    }

    watcher_ = std::make_shared<Watcher>(file_path_, 
        std::bind(&FileWatcher::watch_, this, std::placeholders::_1, std::placeholders::_2), loop);
}

void FileWatcher::watch_(ev::stat &w, int revents) {
    if (!boost::filesystem::is_regular_file(file_path_)) {
        if (last_write_time_ != 0) {
            cb_(file_path_, FILE_DEL);
        }
        return;
    }

    std::time_t t = boost::filesystem::last_write_time(file_path_);
    if (last_write_time_ != t) {
        FileWatcherAction ac = (last_write_time_ == 0) ? FILE_NEW : FILE_MODIFY;
        cb_(file_path_, ac);
    }
}

}
