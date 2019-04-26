#include "EventLoopThread.h"
#include "EventLoop.h"
#include <functional>
#include <assert.h>



EventLoopThread::EventLoopThread():
    loop_(NULL),
    exiting_(false),
    thread_(std::bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
    mutex_(),
    cond_(mutex_)
    {}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != NULL) {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    // 该函数返回新线程中EventLoop对象的地址,因此用条件变量来等待线程的创建与运行
    assert(!thread_.started());
    thread_.start();
    {
        MutexLockGuard lock(mutex_);
        while (loop_ == NULL) {
            cond_.wait();
        }
    }
    
    return loop_;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }

    loop.loop();
}
