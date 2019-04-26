#include "Channel.h"
#include "EventLoop.h"
#include <sys/epoll.h>
#include <iostream>
#include <assert.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;


Channel::Channel(EventLoop *loop, int fd):
    loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    eventHandling_(false)
{  }

Channel::~Channel() {
    assert(!eventHandling_);
}

void Channel::handleEvents() {
    eventHandling_ = true;
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallback_) closeCallback_();
    }
    else if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        // 读事件,对端有数据或者正常关闭
        std::cout << "Channel::handleEvents(): Event EPOLLIN in EventLoop " << loop_ << std::endl;
        if (readCallback_) readCallback_();
    }
    else if (revents_ & EPOLLOUT) {
        std::cout << "Channel::handleEvents(): Event EPOLLOUT in EventLoop " << loop_ << std::endl;
        if (writeCallback_) writeCallback_();
    }
    else {
        std::cout << "Channel::handleEvents(): ERROR in EventLoop " << loop_  << std::endl;
        if (errorCallback_) errorCallback_();
    }
    eventHandling_ = false;
}

void Channel::update() {
    loop_->updateChannel(this);
}

int Channel::getFd() {
    return fd_;
}

void Channel::remove() {
    loop_->removeChannel(this);
}
