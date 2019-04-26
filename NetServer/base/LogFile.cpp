#include "LogFile.h"
#include "FileUtil.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

using namespace std;

LogFile::LogFile(const string& basename, int flushEveryN)
:basename_(basename),
 flushEveryN_(flushEveryN),
 count_(0),
 mutex_(new MutexLock)
{
    cout << "basename:" << basename << endl;
//    assert(basename.find('/') == string::npos);
    time_t now = 0;
    string filename = getLogFileName(basename_, &now);
    file_.reset(new AppendFile(filename));

 }
  
LogFile::~LogFile()
{  }

void LogFile::append(const char *logline, int len) {
    MutexLockGuard lock(*mutex_);
    append_unlocked(logline, len);
}

void LogFile::flush() {
    MutexLockGuard lock(*mutex_);
    file_->flush();
}

void LogFile::append_unlocked(const char* logline, int len) {
    file_->append(logline, len);
    ++count_;
    if (count_ >= flushEveryN_) {
        count_ = 0;
        file_->flush();
    }
}

string LogFile::getLogFileName(const string& basename, time_t* now) {
    string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    filename += ".log";

    return filename;
}
