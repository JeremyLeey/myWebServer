#pragma once

#include <functional>
#include <memory>
#include "base/noncopyable.h"
#include <sys/epoll.h>

class EventLoop;

class Channel: noncopyable {
public:
    typedef std::function<void()> Callback;

    //Channel(EventLoop*);
    Channel(EventLoop *loop, int fd);
    ~Channel();

    int getFd();
    //void setFd(int);


    bool isNoneEvent() const {
        return events_ == static_cast<unsigned int>(kNoneEvent);
    }

    // set callback function
    void setReadCallback(Callback cb) {
        readCallback_ = std::move(cb); // fixme: std::move?
    }
    void setWriteCallback(Callback cb) {
        writeCallback_ = std::move(cb);
    }
    void setErrorCallback(Callback cb) {
        errorCallback_ = std::move(cb);
    }
    void setCloseCallback(Callback cb) {
        closeCallback_ = std::move(cb);
    }

    void handleEvents();
    /*
    void handleEvents() {
        events_ = 0;
        if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
            events_ = 0;
            return;
        }
        if (revents_ & EPOLLERR) {
            if (errorCallback_) errorCallback_();
            events_ = 0;
            return;
        }
        if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
            handleRead();
        }
        if (revents_ & EPOLLOUT) {
            handleWrite();
        }
    }

    */

    //void handleRead();
    //void handleWrite();
    //void handleError(int fd, int err_num, std::string short_msg);

    void setRevents(__uint32_t ev) {
        revents_ = ev;
    }

    void setEvents(__uint32_t ev) {
        events_ = ev;
    }
    
    __uint32_t& getEvents() {
        return events_;
    }

    void enableReading() {events_ |= kReadEvent; update();}
    void enableWriting() {events_ |= kWriteEvent; update();}
    void disableWriting() {events_ &= kWriteEvent; update();}
    void disableAll() {events_ |= kNoneEvent; update();}
    bool isWriting() const {return events_ & kWriteEvent;}



    void remove();
private:
    void update();
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;                    // 每一个Channel都属于一个EventLoop
    int fd_;                            // Channel是对fd的封装
    __uint32_t events_;                 // Channel所关心的IO事件,由用户设置
    __uint32_t revents_;                // 目前活动的事
    int index_;                         // 由EPoll使用,用于标记EPOLL_CTL_ADD/MOD/DEL
    bool eventHandling_;

    Callback readCallback_;
    Callback writeCallback_;
    Callback errorCallback_;
    Callback closeCallback_;
};
