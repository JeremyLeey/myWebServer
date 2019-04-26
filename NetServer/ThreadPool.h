#pragma once

#include "base/MutexLock.h"
#include "base/Condition.h"
#include "base/Thread.h"
#include "base/noncopyable.h"

#include <vector>
#include <deque>


class ThreadPool: noncopyable {
public:
    typedef std::function<void()> Task;

    explicit ThreadPool(const std::string& nameArg = std::string("ThreadPool"));
    ~ThreadPool();

    void setMaxQueueSize(int maxSize) {maxQueueSize_ = maxSize;}
    void setThreadInitCallback(const Task& cb) {
        threadInitCallback_ = cb;
    }

    void start(int numThreads);
    void stop();

    size_t queueSize() const;

    void run(const Task& f);

private:
    bool isFull() const;
    void runInThread();
    Task take();

    mutable MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    string name_;
    Task threadInitCallback_;

    std::vector<std::shared_ptr<Thread>> threads_;
    std::deque<Task> queue_;
    size_t maxQueueSize_;
    bool running_;

}
