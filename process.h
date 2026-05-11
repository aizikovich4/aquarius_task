#pragma once

#include <stdint.h>

typedef enum {
    SUCCESS = 0,
    ERROR_OPEN_FILE,
    ERROR_READ_FILE
} HandleResult;

void asserts();
HandleResult countQwertyNeighborhoodWords(const char* path, uint64_t* resCount);
