#include "codecB.h"

void codecB_encode(char *str) {
    while (*str) {
        *str = (*str + 3) % 256; // Add 3 to ASCII value, wrap around at 256
        str++;
    }
}

void codecB_decode(char *str) {
    while (*str) {
        *str = (*str - 3 + 256) % 256; // Subtract 3 from ASCII value, wrap around at 256
        str++;
    }
}
