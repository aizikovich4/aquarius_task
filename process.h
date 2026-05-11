#pragma once

#include <stdint.h>

typedef enum {
    SUCCESS = 0,
    ERROR_OPEN_FILE,
    ERROR_READ_FILE
} HandleResult;

#ifdef DEBUG
void asserts();
#endif

// This function does a lot of things, opening/closing file, reading context, trimming the spaces - i dont like it,
// but I want to focus on solving the problem, so it'll all be here.
HandleResult countQwertyNeighborhoodWords(const char* path, uint64_t* resCount);
