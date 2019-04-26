#pragma once

#include <functional>
#include <string>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Socket.h"



class EventLoop;
class Channel;
class Buffer;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(const TcpConnectionPtr&)> Callback;
    typedef std::function<void(const TcpConnectionPtr&, std::string&)> MessageCallback;
    
    TcpConnection(EventLoop *, int, const sockaddr_in &);
    ~TcpConnection();

    // 获取当前连接的fd
    int getFd() const {return fd_;}
    // 获取当前连接所属的loop
    EventLoop* getLoop() const {return loop_;}
    
    // 发送数据的函数
    void send(const std::string);
    void sendInLoop(const std::string);

    // 主动清理连接
    void shutDown();
    void shutDownInLoop();

    // channel事件回调
    void handleRead();
    void handleWrite();
    void handleError();
    void handleClose();

    // called when TcpServer accepts a new connection
    void connectEstablished();
    // called when TcpServer has removed me from its map
    void connectDestroyed();

    void setConnectionCallback(const Callback& cb) {
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }
    void setCloseCallback(const Callback& cb) {
        closeCallback_ = cb;
    }

    sockaddr_in getAddress() const {
        return clientaddr_;
    }

private:
    EventLoop *loop_;
    int fd_;
    Socket cliSocket_;
    Channel* channel_;
    sockaddr_in clientaddr_;
    MessageCallback messageCallback_;
    Callback connectionCallback_;
    Callback closeCallback_;

    std::string inputBuffer_;
    std::string outputBuffer_;

};
