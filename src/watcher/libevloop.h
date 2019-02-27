/*************************************************************************
    > File Name: libevloop.h
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Wed 30 Jan 2019 03:52:39 PM CST
 ************************************************************************/
#ifndef WATCHER_LIBEVLOOP_H
#define WATCHER_LIBEVLOOP_H

#ifndef EV_MULTIPLICITY
#define EV_MULTIPLICITY 1
#endif

#include <mutex>
#include "libev/ev++.h"

namespace filewatcher {

class LibevLoop {
public:
    LibevLoop();
    ~LibevLoop();

    template<class T>
    friend void bind(T& a, LibevLoop* b);

public:
    void run_loop();

private:
    ev::dynamic_loop loop_;
    std::timed_mutex timed_mutex_;
};

template<class T>
void bind(T& a, LibevLoop* b) {
    a.set(b->loop_);
}

}

#endif // WATCHER_LIBEVLOOP_H
