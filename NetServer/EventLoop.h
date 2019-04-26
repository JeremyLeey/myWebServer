#pragma once
#include "base/Thread.h"
#include "base/CurrentThread.h"
#include "base/noncopyable.h"
#include <vector>
#include <memory>   // for std::shared_ptr
#include <functional> // for std::function

class Channel;
class Epoll;

class EventLoop: noncopyable {
public:
    typedef std::shared_ptr<Channel> ChannelPtr; 
    typedef std::function<void()> Functor;
    
    EventLoop();
    ~EventLoop();

    void loop();
    void assertInLoopThread();
    bool isInLoopThread() const {return threadId_ == CurrentThread::tid();}
    void wakeup();

    void runInLoop(Functor&&);
    void queueInLoop(Functor&&);
    void quit();

    void updateChannel(Channel*);
    void removeChannel(Channel*);

private:
    void abortNotInLoopThread();
    void handleRead(); // wakeup
    void doPendingFunctors();

    bool looping_;                      // 标记循环开始
    std::shared_ptr<Epoll> poller_;
    int wakeupFd_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;
    const pid_t threadId_;
    ChannelPtr pwakeupChannel_; // fix me : std::unique_ptr
    mutable MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;
};

