#include <stdio.h>

#include "process.h"
#include <unistd.h>  // для getcwd на Linux

int main(int argc, char *argv[]) {
    //asserts();

    if (argc < 2) {
        printf("Example of usage: %s <path_to_file>\n", argv[0]);
        return 1;
    }

    uint64_t resCount = 0;
    HandleResult res = countQwertyNeighborhoodWords(argv[1], &resCount);

    if (res == SUCCESS) {
        printf("Count of enable words (on qwerty limit): %lu\n",resCount);
    } else {
        perror("Error on counting words");
    }

    return 0;
}
