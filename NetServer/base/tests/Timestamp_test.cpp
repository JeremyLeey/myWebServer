#include "Timestamp.h"
#include <iostream>

using namespace std;

int main() {
    for (int i = 0; i < 5; i++)
        cout << Timestamp::now().toFormattedString() << endl;
}
