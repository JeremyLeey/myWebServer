#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"
#include <assert.h>


EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, int numThreads)
    :baseLoop_(baseLoop),
    started_(false),
    numThreads_(numThreads),
    next_(0)
{
    for (int i = 0; i < numThreads_; i++) {
        EventLoopThread *t = new EventLoopThread();
        threads_.push_back(t);
    }
}



EventLoopThreadPool::~EventLoopThreadPool() {
    for (int i = 0; i < numThreads_; i++)
        delete threads_[i];
}

void EventLoopThreadPool::start() {
    baseLoop_->assertInLoopThread();
    started_ = true;
    for (int i = 0; i < numThreads_; i++) {
        threads_[i]->startLoop();
    }
}

EventLoop *EventLoopThreadPool::getNextLoop() {
    baseLoop_->assertInLoopThread();
    assert(started_);
    EventLoop *loop = baseLoop_;

    if (numThreads_ > 0) {
        loop = threads_[next_]->getLoop();
        next_ = (next_+1) % numThreads_;
    }
    return loop;
}
