#pragma once
#include "CountDownLatch.h"
#include "MutexLock.h"
#include "Thread.h"
#include "LogStream.h"
#include "noncopyable.h"
#include <functional>
#include <string>
#include <vector>

// 异步日志类
class AsyncLogging: noncopyable {
public:
    AsyncLogging(const std::string basename, int flushInterval=2);
    ~AsyncLogging() {
        if (running_)
            stop();
    }
    // 前段每生成一条消息, 就调用该函数写到缓冲中
    void append(const char* logline, int len);

    void start() {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop() {
        running_ = false;
        cond_.notify();
        thread_.join();
    }
    
private:
    void threadFunc();
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::shared_ptr<Buffer> BufferPtr;
    typedef std::vector<BufferPtr> BufferVector;

    const int flushInterval_;
    bool running_;
    std::string basename_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer_;                       // 当前缓冲
    BufferPtr nextBuffer_;                          // 预备缓冲
    BufferVector buffers_;                          // 待写入文件的已填满的缓冲
    CountDownLatch latch_;

};
