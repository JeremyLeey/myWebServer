#pragma once

// 服务器socket类,封装socket描述符及相关的初始化操作

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
    // 服务器socket文件描述符
    int serverfd_;
public:
    Socket();
    ~Socket();

    // 获取fd
    int fd() const {return serverfd_;}
    // 设置地址重用
    void setReuseAddr();
    // 设置非阻塞
    void setNonblocking();
    // 绑定地址
    bool bindAddress(int serverport);
    // 开启监听
    bool listen();
    // accept获取连接
    int accept(struct sockaddr_in &clinetaddr);
    // 关闭服务器fd
    bool close();

};
