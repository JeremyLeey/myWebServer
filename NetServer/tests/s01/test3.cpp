#include "EventLoop.h"
#include "Channel.h"
#include <sys/timerfd.h>
#include <iostream>
#include <cstring>


using namespace std;

EventLoop *g_loop;
int timerfd;

void timeout() {
    printf("Timeout\n");
    uint64_t one;
    int n = read(timerfd, &one, sizeof(one));
    if (n != sizeof(uint64_t)) {
        cout << "read error" << endl;
    }
}

int main() {
    EventLoop loop;
    g_loop = &loop;

    timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
    Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec howlong;
    memset(&howlong, 0, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();

    ::close(timerfd);
}
