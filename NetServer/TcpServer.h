#pragma once

#include <functional>
#include <string>
#include <map>
#include <memory>
#include "Socket.h"

#define MAXCONNECTION 20000


class EventLoop;
class Channel;
class TcpConnection;
class EventLoopThreadPool;

class TcpServer{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

    typedef std::function<void(const TcpConnectionPtr&, std::string&)> MessageCallback;
    typedef std::function<void(const TcpConnectionPtr&)> Callback;

    TcpServer(EventLoop* loop, const int port, const int threadnum = 0);
    ~TcpServer();

    // 启动Tcp服务器
    void start();
    // 业务注册函数,注册新连接回调函数,这是提供给用户的接口
    void setConnectionCallback(const Callback &cb) {
        newConnectionCallback_ = cb;
    }
    // 注册数据回调函数
    void setMessageCallback(const MessageCallback &cb) {
        messageCallback_ = cb;
    }

    void setCloseCallback(const Callback &cb) {
        closeCallback_ = cb;
    }

    // 注册数据发送完成回调函数
    // void SetSendCompleteCallback(const Callback &cb) {
    //    sendCompleteCallback_ = cb;
    // }
    // 注册连接异常回调函数
    // void SetErrorCallback(const Callback &cb) {
    //    errorCallback_ = cb;
    // }
private:
    // 服务器的Socket
    Socket serverSocket_;
    // 主loop
    EventLoop *loop_;
    // 服务器事件
    Channel* serverChannel_;
    // 连接数量统计
    int connCount_;
    // TCP连接表
    std::map<int, TcpConnectionPtr> tcpConnlist_;
    // IO线程池
    EventLoopThreadPool* threadpool_;
    //  MutexLock mutex_;
    // 业务接口回调函数
    Callback newConnectionCallback_;
    MessageCallback messageCallback_;
    Callback closeCallback_;

    // 服务器对新连接连接处理的函数
    void newConnection();
    // 移除TCP连接函数
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
};
