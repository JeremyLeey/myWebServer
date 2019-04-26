#pragma once

#include <functional>
#include <memory>
#include "base/noncopyable.h"
#include <sys/epoll.h>

class EventLoop;

class Channel: noncopyable {
public:
    typedef std::function<void()> Callback;

    Channel(EventLoop *loop, int fd);
    ~Channel();

    int getFd();
    bool isNoneEvent() const {
        return events_ == static_cast<unsigned int>(kNoneEvent);
    }

    // set callback function
    void setReadCallback(Callback cb) {
        readCallback_ = std::move(cb); // 此处采用std::move的原因:效率更高,why?
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

    void setRevents(__uint32_t ev) {
        revents_ = ev;
    }

    void setEvents(__uint32_t ev) {
        events_ = ev;
    }
    
    __uint32_t& getEvents() {
        return events_;
    }

    // enableXXX() 注册感兴趣的事件,调用update更新->添加进EPOLL中监听
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
    bool eventHandling_;

    Callback readCallback_;
    Callback writeCallback_;
    Callback errorCallback_;
    Callback closeCallback_;
};
