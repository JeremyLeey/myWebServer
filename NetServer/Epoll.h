#pragma once

#include "base/noncopyable.h"
#include <vector>
#include <map>
#include <sys/epoll.h>
#include <memory>
#include <unistd.h>

class EventLoop;
class Channel;

class Epoll: noncopyable {
public:
    typedef std::vector<Channel*> ChannelList;
    typedef std::map<int, Channel*> ChannelMap;
    Epoll(EventLoop*);
    ~Epoll();

    void poll(int timeout, ChannelList* activeChannels);

    void updateChannel(Channel*);
    void removeChannel(Channel*);
    void assertInLoopThread();
    int getEpollFd() const {return epollfd_;}

    
private:
    void update(int op, Channel* channel);
    void fillActiveChannels(int numEvents, ChannelList* activeChannels);
    static const int MAXFDS = 1e5;
    int epollfd_;
    EventLoop* ownerLoop_;
    std::vector<epoll_event> events_;
    ChannelMap channels_;

};

