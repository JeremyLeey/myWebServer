#pragma once

#include "FileUtil.h"
#include "MutexLock.h"
#include "noncopyable.h"
#include <memory>
#include <string>
#include <time.h>

class LogFile: noncopyable {
public:
    LogFile(const std::string& basename, int flushEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();

private:
    void append_unlocked(const char* logline, int len);
    static std::string getLogFileName(const std::string& basename, time_t* now);

    const std::string basename_;
    const int flushEveryN_;

    int count_;
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;
};
