#include <stdio.h>
#include "hello.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <name>\n", argv[0]);
    } else {
        hello(argv[1]);
    }
    return 0;
}