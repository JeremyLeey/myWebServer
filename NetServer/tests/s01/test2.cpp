#include "EventLoop.h"
#include "base/Thread.h"
#include <iostream>

using namespace std;

EventLoop* g_loop = nullptr;
void threadFunc() {
    g_loop->loop();
}
int main() {
    EventLoop loop;
    g_loop = &loop;
    Thread t(threadFunc);
    t.start();
    t.join();
    return 0;
}
