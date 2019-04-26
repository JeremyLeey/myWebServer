#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"

#include "base/Timestamp.h"


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
    std::cout << Timestamp::now().toFormattedString() << " EventLoop created " << this << " in thread " << threadId_ << std::endl;
    if (t_loopInThisThread) {
        std::cout << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << threadId_ << std::endl;
    } else {
        t_loopInThisThread = this;
    }

    // pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
    pwakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    pwakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    std::cout << "EventLoop " << this << " of thread " << threadId_ << " destructs in thread "<< CurrentThread::tid() << std::endl;
    pwakeupChannel_->disableAll();
    pwakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        std::cerr << "EventLoop::wakeup() writes " << n << " bytes instead of 8" << std::endl;
    }
}

// eventfd的可读回调函数
void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        // Log something
    }
    std::cout << Timestamp::now().toFormattedString() << " Leave EventLoop::handleRead()" << std::endl;
}

void EventLoop::runInLoop(Functor&& cb) {
    if (isInLoopThread())
        cb();
    else
        queueInLoop(std::move(cb));

    std::cout << Timestamp::now().toFormattedString() << " EventLoop " << this << " Leave runInLoop" << std::endl;
}

void EventLoop::queueInLoop(Functor&& cb) {
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
    std::cout << "abortNotInLoopThread()" << std::endl;
    abort();
}


void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    std::vector<Channel*> activeChannels_;

    while (!quit_) {
        std::cout << Timestamp::now().toFormattedString() << " EventLoop "<< this <<" Begin Loop" << std::endl;
        activeChannels_.clear();
        poller_->poll(-1, &activeChannels_);
        std::cout << Timestamp::now().toFormattedString() << " EventLoop " << this << " wake up from epoll_wait" << std::endl;
        eventHandling_ = true;
        for (auto &channel : activeChannels_) {
            if (channel->getFd() == wakeupFd_) {
                std::cout << Timestamp::now().toFormattedString() <<" EventLoop " << this << " was waked up by eventfd" << std::endl;
            }
            channel->handleEvents();
        }
        eventHandling_ = false;
        doPendingFunctors();

    }
    
    std::cout << "EventLoop " << this << " stop looping" << std::endl; 
    looping_ = false;
}

void EventLoop::doPendingFunctors() {
    std::cout << Timestamp::now().toFormattedString() << " EventLoop::doPendingFunctors()" << std::endl;
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (size_t i = 0; i < functors.size(); i++) {
        functors[i]();
    }
    callingPendingFunctors_ = false;
    std::cout << Timestamp::now().toFormattedString() << " Leave EventLoop::doPendingFunctors()" << std::endl;
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
