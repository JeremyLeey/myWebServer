#include "Epoll.h"
#include "EventLoop.h"
#include "Channel.h"


#include <sys/epoll.h>
#include <errno.h>
#include <sys/socket.h>
#include <assert.h>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>

const int EVENTSUM = 4096;
const int EPOLLWAIT_TIME=10000;
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;


Epoll::Epoll(EventLoop* loop_):
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
    ownerLoop_(loop_),
    events_(EVENTSUM)
{
    assert(epollfd_ > 0);
}

Epoll::~Epoll() {
    ::close(epollfd_);
}


void Epoll::updateChannel(Channel* channel) {
    assertInLoopThread();
    int fd = channel->getFd();
    if (channels_.find(fd) == channels_.end()) {
        // EPOLL_CTL_ADD
        channels_[fd] = channel;
        update(EPOLL_CTL_ADD, channel);
    } else {
        // EPOLL_CTL_MOD/EPOLL_CTL_DEL
        if (channel->isNoneEvent()) {
            // delete
            channels_.erase(fd);
            update(EPOLL_CTL_DEL, channel);
        } else {
            // mod
            channels_[fd] = channel;
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void Epoll::update(int operation, Channel* channel) {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->getEvents();
    event.data.fd = channel->getFd();
    int fd = channel->getFd();
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0 ) {
        std::cout << "epoll_ctl something wrong" << std::endl;
    }
}

void Epoll::poll(int timeout, ChannelList* activeChannels) {
    int numEvents = ::epoll_wait(epollfd_, &(*events_.begin()), static_cast<int>(events_.size()), timeout);
    if (numEvents > 0) {
        std::cout << numEvents << " events happened in EventLoop " << ownerLoop_ << std::endl;
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == static_cast<int>(events_.size())) {
            events_.resize(numEvents*2);
        }
    } else if (numEvents == 0) {
        std::cout << "Time is limited and nothing happened" << std::endl;
    } else {
        std::cout << "epoll_wait wrong with " << errno << " : " << strerror(errno) << std::endl;
    }
}

void Epoll::fillActiveChannels(int numEvents, ChannelList* activeChannels) {
    for (int i = 0; i < numEvents; i++) {
        int fd = events_[i].data.fd;
        Channel* channel = channels_[fd];
        channel->setRevents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void Epoll::removeChannel(Channel* channel) {
    if (channels_.find(channel->getFd()) != channels_.end()) {
        channels_.erase(channel->getFd());
        update(EPOLL_CTL_DEL, channel);
    }
}

void Epoll::assertInLoopThread() {
    ownerLoop_->assertInLoopThread();
}




