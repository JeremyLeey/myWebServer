#include "TcpServer.h"
#include "Channel.h"
#include "TcpConnection.h"

#include "EventLoop.h" // ?
#include "EventLoopThreadPool.h"
#include "base/Timestamp.h"


#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <netinet/in.h> // for sockaddr_in
#include <cstring> // for memset
#include <arpa/inet.h>

void setNonblocking(int);


TcpServer::TcpServer(EventLoop* loop, const int port, const int threadnum)
    :serverSocket_(),
    loop_(loop),
    serverChannel_(new Channel(loop, serverSocket_.fd())),
    connCount_(0),
    threadpool_(new EventLoopThreadPool(loop, threadnum))    
{

    serverSocket_.setReuseAddr();
    serverSocket_.setNonblocking();
    serverSocket_.bindAddress(port);
    serverSocket_.listen();

    serverChannel_->setReadCallback(std::bind(&TcpServer::newConnection, this));
    //serverChannel_.setErrorHandle(std::bind(&TcpServer::connectionError, this));
}

TcpServer::~TcpServer() {
    // socket.close() ? 
    std::cout << "TcpServer shutDown" << std::endl;
}

void TcpServer::start() {
     threadpool_->start();
    serverChannel_->enableReading();
}

void TcpServer::newConnection() {
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    int clientFd = serverSocket_.accept(clientAddr);
    if (++connCount_ >= MAXCONNECTION) {
        ::close(clientFd);
        return;
    }
    setNonblocking(clientFd);

    // 选择IO线程loop
    EventLoop *ioLoop = threadpool_->getNextLoop();
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(ioLoop, clientFd, clientAddr);
    conn->setConnectionCallback(newConnectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    tcpConnlist_[clientFd] = conn;

    
    std::cout << Timestamp::now().toFormattedString() << " ioLoop->runInLoop(TcpConnection::connectEstablished)" << std::endl;
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}


void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
    //loop_->assertInLoopThread();
    std::cout << Timestamp::now().toFormattedString() << " loop_->runInLoop(TcpServer::removeConnectionInLoop)" << std::endl;
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
    loop_->assertInLoopThread();
    std::cout << Timestamp::now().toFormattedString() << " TcpServer::removeConnectionInLoop [" << conn->getFd() << "]-connection in EventLoop" << loop_ <<  std::endl;
    --connCount_;
    tcpConnlist_.erase(conn->getFd());
    EventLoop* ioLoop = conn->getLoop();
    std::cout << Timestamp::now().toFormattedString() << " ioLoop->runInLoop(TcpConnectionDestroyed)" << std::endl;
    ioLoop->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void setNonblocking(int fd) {
    int opts = fcntl(fd, F_GETFL);
    if (opts < 0) {
        perror("fcntl(fd, GETFL)");
        exit(1);
    }
    if (fcntl(fd, F_SETFL, opts | O_NONBLOCK) < 0) {
        perror("fcntl(fd, SETFL, opts)");
        exit(1);
    }
}
