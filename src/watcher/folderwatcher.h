/*************************************************************************
    > File Name: folderwatcher.h
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Fri 25 Jan 2019 07:02:46 PM CST
 ************************************************************************/
#ifndef WATCHER_FOLDERWATCHER_H
#define WATCHER_FOLDERWATCHER_H

#ifndef EV_MULTIPLICITY
#define EV_MULTIPLICITY 1
#endif

#include "libevloop.h"
#include "folderdiff.h"
#include "filewatcher.h"
#include "libev/ev++.h"

#include <map>
#include <mutex>
#include <memory>
#include <mutex>
#include <string>
#include <functional>

namespace filewatcher {

enum WatcherAction {
    UNKNOWN = 0,
    NEW,
    DEL,
    MODIFY,
};

class FolderWatcher {
public:
    using callback = std::function<void(const PathInfo& pi, WatcherAction action)>;
    
    FolderWatcher() = delete;
    explicit FolderWatcher(const std::string& path, callback c, LibevLoop* loop = nullptr);
    ~FolderWatcher();
private:
    void watch_(ev::stat&, int);
    void watch_folder_(const std::string& path);
    void watch_file_(const std::string& path);

    void file_watcher_(const std::string& path, FileWatcherAction action);

    void notify_folderwatcher_change_(const PathInfo& pi, bool add);
    void notify_filewatcher_change_(const PathInfo& pi, WatcherAction action);

    void change_filewatchers_(const std::string& path, WatcherAction action);

private:
    callback cb_;
    std::string folder_path_;
    std::shared_ptr<Watcher> watcher_;

    std::shared_ptr<FolderDiff> fdiff_;
    std::map<std::string, std::shared_ptr<FileWatcher>> file_watchers_;

    std::map<std::string, std::time_t> files_last_modify_time_;

    std::mutex mutex_;
};

}

#endif // WATCHER_FOLDERWATCHER_H
