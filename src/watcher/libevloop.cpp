/*************************************************************************
    > File Name: libevloop.cpp
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Wed 30 Jan 2019 03:52:44 PM CST
 ************************************************************************/
#include "libevloop.h"

#include <chrono>
#include <thread>
#include <glog/logging.h>

namespace filewatcher {

LibevLoop::LibevLoop() {
}

LibevLoop::~LibevLoop() {
    loop_.break_loop(ev::ALL);
}

void LibevLoop::run_loop_() {
    if (timed_mutex_.try_lock_for(std::chrono::milliseconds(1))) {
        std::thread t([this]{
            timed_mutex_.lock();
//            LOG(INFO) << "loop run";
            loop_.run(); 
//            LOG(INFO) << "loop stoped";
            timed_mutex_.unlock();
        });
        t.detach();
        timed_mutex_.unlock();
    }
}

}
