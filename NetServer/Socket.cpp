#include "Socket.h"

Socket::Socket() {
    serverfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd_ < 0) {
        perror("socket create fail!");
        exit(-1);
    }
    std::cout << "Server create socket " << serverfd_ << std::endl;
}

Socket::~Socket() {
    ::close(serverfd_);
    std::cout << "Server close socket" << std::endl;
}

void Socket::setReuseAddr() {
    int on = 1;
    ::setsockopt(serverfd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

void Socket::setNonblocking() {
    int opts = fcntl(serverfd_, F_GETFL);
    if (opts < 0) {
        perror("fcntl(serverfd_, GETFL) error");
        exit(1);
    }
    if (fcntl(serverfd_, F_SETFL, opts | O_NONBLOCK) < 0) {
        perror("fcntl(serverfd_, SETFL, opts) error");
        exit(1);
    }
    std::cout << "Server setNonblocking..." << std::endl;
}

bool Socket::bindAddress(int port) {
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);

    int resval = ::bind(serverfd_,  (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    if (resval == -1) {
        ::close(serverfd_);
        perror("error bind");
        exit(1);
    }

    std::cout << "server bindAddress ..." << std::endl;
    return true;
}

bool Socket::listen() {
    if (::listen(serverfd_, 8192) < 0) {
        perror("error listen");
        ::close(serverfd_);
        exit(1);
    }

    std::cout << "Server listening ..." << std::endl;
    return true;
}

int Socket::accept(struct sockaddr_in &clientaddr) {
    socklen_t len = sizeof(clientaddr);
    int clientfd = ::accept(serverfd_,  (struct sockaddr*)&clientaddr, &len);
    assert(clientfd >= 0);
    return clientfd;
}

bool Socket::close() {
    ::close(serverfd_);
    std::cout << "server close socket ..." << std::endl;
    return true;
}
