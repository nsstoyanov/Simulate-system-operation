#include "System.h"
#include <iostream>
using namespace std;

int main() {
    const char* control = "RRCCRCC";

    System sys(control);
    sys.run();

    return 0;
}
