#include "ThreadPool.h"
#include <functional>

using namespace std;

ThreadPool::ThreadPol(const string& nameArg)
    :mutex_(),
    notEmpty_(mutex_),
    notFull(mutex_),
    name_(nameArg),
    maxQueueSize_(0),
    running(false)
{

}

ThreadPool::~ThreadPool() {
    if (running_) {
        stop();
    }
}


void ThreadPool::start(int numThreads) {
    assert(threads_.empty());
    running_ = true;

    threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; i++) {
        char id[32];
        snprintf(id, sizeof id, "%d", i+1);
        shared_ptr<Thread> ThreadPtr(new Thread(bind(&ThreadPool::runInThread, this), name_+id));
        threads_.push_back(ThreadPtr);
        threads_[i].start();
    }

    if (numThreads == 0 && threadInitCallback_) {
        threadInitCallback_();
    }
}

void ThreadPool::stop() {
    {
        MutexGuard lock(mutex_);
        running = false;
        notEmpty_.notifyAll();
    }
    for_each(threads_.begin(), threads_.end(), bind(Thread::join, _1));
}

void ThreadPool::runInThread() {
    if (threadInitCallback_) {
        threadInitCallback_();
    }

    while (running_) {
        Task task(take());
        if (task) {
            task();
        }

    }
}

ThreadPool::Task ThreadPool::take() {
    MutexLockGuard lock(mutex_);
    while (queue_.empty() && running_) {
        notEmpty_.wait();
    }
    Task task;
    if (!queue_.empty()) {
        task = queue_.front();
        queue_.pop_front();
        if (maxQueueSize_ > 0) {
            notFull_.notify();
        }
    }
    return task;
}

void ThreadPool:;run(const Task& task) {
    if (threads_.empty()) {
        task();
    } else {
        MutexLockGuard lock(mutex_);
        while (!isFull()) {
            notFull_.wait();
        }
        assert(!isFull());

        queue_.push_back(task);
        notEmpty_.notify();
    }
}

bool ThreadPool::isFull() const {
    return maxQueueSize_ > 0 && queue_size() >= maxQueueSize_;

}

size_t ThreadPool::queueSize() const {
    MutexLockGuard lock(mutex_);
    return queue_.size();
}



