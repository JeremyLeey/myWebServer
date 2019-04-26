#include "ThreadPool.h"

using namespace std;

void task() {
    int c = 1;
    for (int i = 0; i < 100000; i++) {
        c++;
    }
}

int main() {
    ThreadPool pool;
    pool.start(5);
    printf("ThreadPool Construction Done\n");

    for (int i = 0; i < 10; i++) {
        pool.run(task);
    }

    while (1) {
        int remain = pool.getQueueSize();
        printf("There are still %d task need to handle\n", remain);
        if (remain == 0) break;
        sleep(2);
        printf("2 seconds later\n");
    }
    return 0;
}
