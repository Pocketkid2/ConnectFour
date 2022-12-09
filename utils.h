#ifndef CONNECT_FOUR_UTILS_H
#define CONNECT_FOUR_UTILS_H

#include <stdint.h>
#include <stdlib.h>

// Converts a 64-bit bitboard into a string
char *bitboard_to_str(uint64_t bitboard) {
    char *bytes = calloc(65, 1);
    for (int i = 0; i < 64; i++) {
        bytes[i] = (bitboard & (1L << (63 - i))) ? '1' : '0';
    }
    return bytes;
}

#endif