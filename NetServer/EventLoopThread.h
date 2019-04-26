#pragma once
#include "base/Condition.h"
#include "base/MutexLock.h"
#include "base/Thread.h"
#include "base/noncopyable.h"

/*
 * IO线程不一定是主线程,我们可以在任何一个线程创建并运行EventLoop
 * 一个程序也可以有不止一个IO线程,我们可以按优先级将不同的socket分给不同的IO线程
 * 为了方便将来使用,我们定义EventLoopThread class，这正是one loop per thread的本意
 * EventLoopThread会启动自己的线程,并在其中运行EventLoop::loop()
 */

class EventLoop;

class EventLoopThread: noncopyable {
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();
    EventLoop* getLoop() const {return loop_;}
private:
    void threadFunc();
    EventLoop *loop_;
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
};
