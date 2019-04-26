#include "EventLoop.h"
#include "base/Thread.h"
#include <iostream>

using namespace std;

void threadFunc() {
    printf("threadFunc(): pid = %d, tid = %d\n",
           getpid(), CurrentThread::tid());
    EventLoop loop;
    loop.loop();
}
int main() {
    printf("main(): pid=%d, tid=%d\n",
           getpid(), CurrentThread::tid());
    EventLoop loop;
    Thread thread(threadFunc);
    thread.start();

    loop.loop();
    pthread_exit(NULL);
}
