#include "Logging.h"
#include "LogFile.h"
#include <bits/stdc++.h>
#include <unistd.h>
#include <libgen.h>

using namespace std;

unique_ptr<LogFile> g_logFile;

int main(int argc, char* argv[]) {

    char name[256] = {0};
    strncpy(name, argv[0], sizeof name -1);
    string t = ::basename(name);
    cout << t << endl;
    g_logFile.reset(new LogFile(t, 200*1000));
    string line = "1234567890 abcdefghijklmn ABCDEFGHIJKLMNOPQRST UVWXYZ";

    printf("Hello,World\n");
    for (int  i = 0; i < 10000; i++) {
        LOG << line << i;
        usleep(1000);
    }
    return 0;
}
