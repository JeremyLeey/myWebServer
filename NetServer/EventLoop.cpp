#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"

#include <sys/eventfd.h>
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <poll.h>

__thread EventLoop* t_loopInThisThread = 0;

int createEventfd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        std::cout << "eventfd error" << std::endl;
        abort();
    }
    return evtfd;
}


EventLoop::EventLoop():
    looping_(false),
    poller_(new Epoll(this)),
    wakeupFd_(createEventfd()),
    quit_(false),
    eventHandling_(false),
    callingPendingFunctors_(false),
    threadId_(CurrentThread::tid()),
    pwakeupChannel_(new Channel(this, wakeupFd_))
{
    std::cout << "EventLoop created " << this << " in thread " << threadId_ << std::endl;
    if (t_loopInThisThread) {
        std::cout << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << threadId_ << std::endl;
    } else {
        t_loopInThisThread = this;
    }
    pwakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    pwakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    std::cout << "EventLoop " << this << " destructs in thread "<< threadId_ << std::endl;
    pwakeupChannel_->disableAll();
    pwakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    assert(n == sizeof(one));
}

// eventfd的可读回调函数
void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof one);
    assert(n == sizeof(one));
}

void EventLoop::runInLoop(Functor&& cb) {
    if (isInLoopThread())
        cb();
    else
        queueInLoop(std::move(cb));
}

void EventLoop::queueInLoop(Functor&& cb) {
    // 有可能在别的线程中访问,因此需要先加锁
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.emplace_back(std::move(cb));
    }
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::assertInLoopThread() {
    if (!isInLoopThread())
        abortNotInLoopThread();
}

void EventLoop::abortNotInLoopThread() {
    abort();
}

void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    std::vector<Channel*> activeChannels_;

    while (!quit_) {
        std::cout << "EventLoop "<< this <<" Begin Loop" << std::endl;
        activeChannels_.clear();
        poller_->poll(-1, &activeChannels_);
        std::cout << "EventLoop " << this << " wake up from epoll_wait" << std::endl;
        eventHandling_ = true;
        for (auto &channel : activeChannels_) {
            channel->handleEvents();
        }
        eventHandling_ = false;
        doPendingFunctors();

    }
    std::cout << "EventLoop " << this << " stop looping" << std::endl; 
    looping_ = false;
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    // 缩小临界区
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (size_t i = 0; i < functors.size(); i++) {
        functors[i]();
    }
    callingPendingFunctors_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::removeChannel(Channel* channel) {
    poller_->removeChannel(channel);
}

void EventLoop::updateChannel(Channel* channel) {
    poller_->updateChannel(channel);
}
