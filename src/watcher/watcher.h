/*************************************************************************
    > File Name: watcher.h
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Thu 31 Jan 2019 08:18:05 PM CST
 ************************************************************************/
#ifndef WATCHER_WATCHER_H
#define WATCHER_WATCHER_H

#ifndef EV_MULTIPLICITY
#define EV_MULTIPLICITY 1
#endif

#include "libevloop.h"
#include "libev/ev++.h"

#include <string>
#include <functional>

namespace filewatcher {

class Watcher {
public:
    using callback = std::function<void(ev::stat&, int)>;
    
    Watcher() = delete;
    explicit Watcher(const std::string& path, callback c, LibevLoop* loop = nullptr);
    ~Watcher();
private:
    void callback_(ev::stat &w, int revents);
private:
    callback cb_;
    ev::stat state_;
};

}

#endif // WATCHER_WATCHER_H
