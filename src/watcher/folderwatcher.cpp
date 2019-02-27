/*************************************************************************
    > File Name: folderwatcher.h
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Fri 25 Jan 2019 07:02:46 PM CST
 ************************************************************************/
#include "folderwatcher.h"
#include "folderdiff.h"
#include "utils/file_util.h"
#include "utils/folderscan.h"

#include <thread>
#include <iostream>
#include <glog/logging.h>
#include <boost/filesystem.hpp>

namespace filewatcher {

FolderWatcher::FolderWatcher(const std::string& path, callback c, LibevLoop* loop) {
    folder_path_ = absolute(path);
    if (boost::filesystem::is_regular_file(folder_path_)) {
        return;
    }

    cb_ = std::move(c);
    fdiff_ = std::make_shared<FolderDiff>(folder_path_);

    folder_scan(folder_path_, 
        std::bind(&FolderWatcher::watch_file_, this, std::placeholders::_1),
        std::bind(&FolderWatcher::watch_folder_, this, std::placeholders::_1));

    watcher_ = std::make_shared<Watcher>(folder_path_, 
        std::bind(&FolderWatcher::watch_, this, 
            std::placeholders::_1, std::placeholders::_2), loop);
}

FolderWatcher::~FolderWatcher() {
}

void FolderWatcher::watch_(ev::stat &w, int revents) {
    PathInfoSet add;
    PathInfoSet remove;
    fdiff_->diff(add, remove);
    for (auto& it : add) {
        notify_change_(it, true);
    }

    for (auto& it : remove) {
        notify_change_(it, false);
    }
}

void FolderWatcher::watch_folder_(const std::string& path) {
    std::unique_lock<std::mutex> lock(mutex_);
    folder_watchers_[path] = std::make_shared<Watcher>(path, 
        std::bind(&FolderWatcher::watch_, this, 
            std::placeholders::_1, std::placeholders::_2));
}

void FolderWatcher::watch_file_(const std::string& path){
    std::unique_lock<std::mutex> lock(mutex_);
    files_last_modify_time_[path] = boost::filesystem::last_write_time(path);
    file_watchers_[path] = std::make_shared<FileWatcher>(path, 
        std::bind(&FolderWatcher::file_watcher_, this, 
            std::placeholders::_1, std::placeholders::_2));
}

void FolderWatcher::file_watcher_(const std::string& path, FileWatcherAction action) {
    PathInfo pi{path, E_FILE};
    WatcherAction ac = (WatcherAction)action; 
    notify_filewatcher_change_(pi, ac);
}

void FolderWatcher::notify_change_(const PathInfo& pi, bool add) {
    if (pi.type == E_FOLDER) {
        notify_folderwatcher_change_(pi, add ? NEW : DEL);
    }
    else {
        notify_filewatcher_change_(pi, add ? NEW : DEL);
    }
}

void FolderWatcher::notify_folderwatcher_change_(const PathInfo& pi, WatcherAction action) {
    bool notify = true;
    if (action == DEL) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = folder_watchers_.find(pi.path);
        if (it == folder_watchers_.end()) {
            notify = false;
        }
        else {
            folder_watchers_.erase(it);
        }
    }
    else if (action == NEW || action == MODIFY) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = folder_watchers_.find(pi.path);
        if (it == folder_watchers_.end()) {
            folder_watchers_[pi.path] = std::make_shared<Watcher>(pi.path, 
                std::bind(&FolderWatcher::watch_, this, 
                    std::placeholders::_1, std::placeholders::_2));
        }
    }
    
    if (notify) {
        cb_(pi, action);
    }
}

void FolderWatcher::notify_filewatcher_change_(const PathInfo& pi, WatcherAction action) {
    change_filewatchers_(pi.path, action);

    bool notify = true;
    if (action == DEL) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = files_last_modify_time_.find(pi.path);
        if (it == files_last_modify_time_.end()) {
            notify = false;
        }
        else {
            files_last_modify_time_.erase(it);
        }
    }
    else if (action == NEW || action == MODIFY) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = files_last_modify_time_.find(pi.path);
        if (it != files_last_modify_time_.end()) {
            if (boost::filesystem::last_write_time(pi.path) == it->second) {
                notify = false;
            }
        }
        else {
            files_last_modify_time_[pi.path] = boost::filesystem::last_write_time(pi.path);
        }
    }
    
    if (notify) {
        cb_(pi, action);
    }
}

void FolderWatcher::change_filewatchers_(const std::string& path, WatcherAction action) {
    if (action == DEL) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = file_watchers_.find(path);
        if (it != file_watchers_.end()) {
            file_watchers_.erase(it);
        }
    }
    else if (action == NEW) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = file_watchers_.find(path);
        if (it != file_watchers_.end()) {
            file_watchers_.erase(it);
        }
        file_watchers_[path] = std::make_shared<FileWatcher>(path, 
            std::bind(&FolderWatcher::file_watcher_, this, 
                std::placeholders::_1, std::placeholders::_2));
    }
}

}
