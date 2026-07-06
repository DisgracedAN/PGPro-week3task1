#include <stdio.h>
#include <string.h>
#include<windows.h>
#include "tests.h"



int main(int argc, char* argv[]) {
    SetConsoleOutputCP(65001);
    if (argc == 2 && strcmp(argv[1], "--test") == 0) {
        run_automatic_tests();
    }
    else if (argc == 2 && strcmp(argv[1], "--animate") == 0) {
        run_animated_demo();
    }
    else {
        run_demo();
    }
    return 0;
}