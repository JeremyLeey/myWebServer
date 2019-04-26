#pragma once

#include <sys/time.h>
#include <string>

class Timestamp{
public:
    Timestamp(int64_t microSecondsSinceEpochArg):
        microSecondsSinceEpoch_(microSecondsSinceEpochArg)
    {

    }

    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds = true) const;
    
    static Timestamp now();

    static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
    int64_t microSecondsSinceEpoch_;
};
