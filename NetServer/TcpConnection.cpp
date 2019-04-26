#include "TcpConnection.h"
#include "EventLoop.h"
#include "Channel.h"

#include <unistd.h> // for read/write
#include <string.h> // for strerror
#include <assert.h>
#include <iostream>

#define BUFSIZE 65536

int readn(int, std::string&);
int sendn(int, std::string&);


TcpConnection::TcpConnection(EventLoop *loop, int sockfd, const struct sockaddr_in& clientaddr)
    :loop_(loop),
    fd_(sockfd),
    cliSocket_(sockfd),
    channel_(new Channel(loop, sockfd)),
    clientaddr_(clientaddr),
    inputBuffer_(),
    outputBuffer_()
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    cliSocket_.setNonblocking();
}
TcpConnection::~TcpConnection() {
    std::cout << "TcpConnection::~TcpConnection(): ok" << std::endl;
}

void TcpConnection::connectEstablished() {
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::send(const std::string s) {
    if (loop_->isInLoopThread()) {
        sendInLoop(s);
    } else {
        loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, s));
    }
}

void TcpConnection::sendInLoop(const std::string s) {
    loop_->assertInLoopThread();
    int nwrote = 0;
    // if nothing in the output queue,try writing directly
    if (outputBuffer_.size() == 0) {
        nwrote = ::write(fd_, s.data(), s.size());
        if (nwrote < 0) {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                std::cout << "TcpConnection::sendInLoop ERROR" << std::endl;
            }
        }
    }
    assert(nwrote >= 0);
    if (nwrote < s.size()) {
        // 利用epoll关注可写事件
        std::cout << "Use EPOLL to listen writable" << std::endl;
        outputBuffer_.append(s.data()+nwrote, s.size()-nwrote);
        channel_->enableWriting();
    }
}

void TcpConnection::shutDown() {
    loop_->runInLoop(std::bind(&TcpConnection::shutDownInLoop, this));
}

void TcpConnection::shutDownInLoop() {
    loop_->assertInLoopThread();
    cliSocket_.shutdownWrite();
}

void TcpConnection::handleRead() {
    /* old Version
    char buf[1024];
    int n = ::read(channel_->getFd(), buf, sizeof buf);
    */
    int n = readn(fd_, inputBuffer_);
    if (n > 0) {
        messageCallback_(shared_from_this(), inputBuffer_);
    } else if (n == 0) {
        handleClose();
    } else {
        handleError();
    }
}

void TcpConnection::handleWrite() {
    loop_->assertInLoopThread();
    int result = sendn(fd_, outputBuffer_);
    if (result > 0) {
        if (outputBuffer_.size() == 0) {
            channel_->disableWriting();
        }
    } else {
        std::cout << "TcpConnection::handleWrite ERROR" << std::endl;
    }

}

void TcpConnection::handleError() {
    std::cout << "TcpConnection Error with errno = " << errno << " " << strerror(errno) << std::endl;
}

void TcpConnection::handleClose() {
    loop_->assertInLoopThread();
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    loop_->assertInLoopThread();
    /* 在某些情况下可以不经hannClose而直接调用connectDestroyed 
    channel_->disableAll();
    connectionCallback_(shared_from_this());
    */
    loop_->removeChannel(channel_);
}

int readn(int fd, std::string& inputBuffer_) {
    char buffer[BUFSIZE];
    int nbyte = 0; // 一次read调用返回的读取字节数
    int readSum = 0; // 总共读到的字节数
    for (;;) {
        // 循环的原因不是eg,而是数据太多buffer太小,效率太低?
        nbyte = ::read(fd, buffer, BUFSIZE);
        if (nbyte > 0) {
            readSum += nbyte;
            inputBuffer_.append(buffer, nbyte);
            if (nbyte < BUFSIZE) {
                return readSum;
            } else 
                continue;
        } else if (nbyte < 0) {
            // Error
            if (errno == EAGAIN) {
                // 输入缓冲区没有数据了,即刚好读BUFSIZE整数倍
                return readSum;
            } else {
                std::cerr << "READ ERRNO : " << errno << std::endl;
                return -1;
            }
        } else 
            break;
    }
    return 0;
}


int sendn(int fd, std::string& outputBuffer_) {
    int nbyte = 0;
    int sendSum = 0;
    for (;;) {
        nbyte = ::write(fd, outputBuffer_.data(), outputBuffer_.size());
        if (nbyte > 0) {
            sendSum += nbyte;
            outputBuffer_.erase(0, nbyte);
            if (outputBuffer_.size() == 0) {
                return sendSum;
            } else 
                continue;
        } else {
            if (errno == EAGAIN) {
                std::cout << "write errno EAGAIN, not finish!" << std::endl;
                return sendSum;
            } else {
                std::cout << "WRITE ERRNO " << errno << std::endl;
                return -1;
            }
        }
    }
    return sendSum;
}

