#include "Socket.h"

Socket::Socket() {
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ < 0) {
        perror("socket create fail!");
        exit(-1);
    }
}

Socket::Socket(int fd):fd_(fd)
{
    // do nothing
}

Socket::~Socket() {}

void Socket::setReuseAddr() {
    int on = 1;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

// 在新版Linux系统调用中可以一步到位
void Socket::setNonblocking() {
    int opts = fcntl(fd_, F_GETFL);
    if (opts < 0) {
        perror("fcntl(serverfd_, GETFL) error");
        exit(1);
    }
    if (fcntl(fd_, F_SETFL, opts | O_NONBLOCK) < 0) {
        perror("fcntl(serverfd_, SETFL, opts) error");
        exit(1);
    }
    std::cout << "Socket::setNonblocking(): ok" << std::endl;
}

bool Socket::bindAddress(int port) {
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);

    int resval = ::bind(fd_,  (sockaddr*)&serveraddr, sizeof(serveraddr));

    if (resval == -1) {
        ::close(fd_);
        perror("error bind");
        exit(1);
    }

    std::cout << "Socket::bindAddress(): ok" << std::endl;
    return true;
}

bool Socket::listen() {
    if (::listen(fd_, 8192) < 0) {
        perror("error listen");
        ::close(fd_);
        exit(1);
    }

    std::cout << "Socket::listen(): ok" << std::endl;
    return true;
}

int Socket::accept(struct sockaddr_in &clientaddr) {
    socklen_t len = sizeof(clientaddr);
    int clientfd = ::accept(fd_,  (sockaddr*)&clientaddr, &len);
    assert(clientfd >= 0);
    return clientfd;
}

bool Socket::close() {
    ::close(fd_);
    // 检查close返回值
    std::cout << "Socket::close(): ok" << std::endl;
    return true;
}

void Socket::shutdownWrite() {
    // 关闭写端,从而在TCP层面解决当你打算关闭网络时,如何得知对方发了一些数据而你还没收到?
    // 当然也可以在应用层解决,双方商量好不再互发数据,就可以直接断开连接
    int ret = ::shutdown(fd_, SHUT_WR);
    // 检查返回值
    assert(ret == 0);
    std::cout << "Socket::shutdownWrite(): ok" << std::endl;
}
