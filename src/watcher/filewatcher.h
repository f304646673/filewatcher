/*************************************************************************
    > File Name: filewatcher.h
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Wed 30 Jan 2019 03:07:00 PM CST
 ************************************************************************/
#ifndef WATCHER_FILEWATCHER_H
#define WATCHER_FILEWATCHER_H

#include <memory>
#include <string>
#include <functional>

#include "watcher.h"

namespace filewatcher {

enum FileWatcherAction {
    FILE_UNKNOWN = 0,
    FILE_NEW,
    FILE_DEL,
    FILE_MODIFY,
};

class FileWatcher {
public:
    using callback = std::function<void(const std::string& path, FileWatcherAction action)>;

    FileWatcher() = delete;
    ~FileWatcher();
    explicit FileWatcher(const std::string& path, callback cb, LibevLoop* loop = nullptr);
private:
    void watch_(ev::stat&, int);
private:
    callback cb_;
    std::string file_path_;
    std::time_t last_write_time_ = 0;
    std::shared_ptr<Watcher> watcher_;
};

}

#endif // WATCHER_FILEWATCHER_H
