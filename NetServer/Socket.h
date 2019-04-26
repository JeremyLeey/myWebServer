#pragma once

// 套接字描述符封装，socket描述符及相关的初始化操作

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cassert>
#include <fcntl.h>
#include <iostream>
#include <cstring>

class Socket{
private:
    // socket文件描述符
    int fd_;
public:
    Socket();
    Socket(int);
    ~Socket();

    // 获取fd
    int fd() const {return fd_;}
    // 设置地址重用SO_REUSEPORT for server
    void setReuseAddr();
    // 设置非阻塞
    void setNonblocking();
    // 绑定地址 for server
    bool bindAddress(int serverport);
    // 开启监听 for server
    bool listen();
    // accept获取连接
    int accept(struct sockaddr_in &clinetaddr);
    // 关闭
    bool close();
    //
    void shutdownWrite();
};
