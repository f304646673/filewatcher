/*************************************************************************
    > File Name: watcher.cpp
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Thu 31 Jan 2019 08:18:09 PM CST
 ************************************************************************/
#include "watcher.h"
#include "utils/file_util.h"
#include <thread>
#include <glog/logging.h>

namespace filewatcher {

Watcher::Watcher(const std::string& path, callback c, LibevLoop* loop) {
    std::string absolute_path = absolute(path);
    if (!loop) {
        static LibevLoop loop_;
        loop = &loop_;
    }
    cb_.swap(c);
    state_.set<Watcher, &Watcher::callback_>(this);
    bind(state_, loop);
    state_.start(absolute_path.c_str());
    loop->run_loop();
}

Watcher::~Watcher() {
    state_.stop();
}

void Watcher::callback_(ev::stat &w, int revents) {
    cb_(w, revents);
}

}
