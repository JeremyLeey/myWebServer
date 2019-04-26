// 为了避免同一个文件被include多次,类似头文件预编译宏
#pragma once
#include "CountDownLatch.h"
#include "noncopyable.h"
#include <functional>
#include <memory>
#include <pthread.h>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>

class Thread: noncopyable {
public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc&, const std::string& name = std::string());
    ~Thread();
    void start();
    int join();
    bool started() const {return started_;}
    pid_t tid() const {return tid_;}
    const std::string& name() const {return name_;}

private:
    void setDefaultName();
    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;                      // 使用gettid(2)系统调用的返回值作为线程id
    ThreadFunc func_;               // 线程入口函数？
    std::string name_;              // 线程名
    CountDownLatch latch_;
};
