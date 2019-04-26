#pragma once
#include <time.h>
#include <stdio.h>

void getTime() {
    char buf[64];
    time_t t;
    struct tm *tmp;
    time(&t);
    tmp = localtime(&t);
    strftime(buf, 64, "time and date: %r", tmp);
    printf("%s ", buf);
}
