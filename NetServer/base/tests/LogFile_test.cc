#include "LogFile.h"
#include "Logging.h"

#include <unistd.h>

using namespace std;

// unique_ptr<LogFile> g_logFile;


int main(int argc, char* argv[]) {
    int i = 2;
    LOG << "Hello, World "<< i; 
    return 0;
}
