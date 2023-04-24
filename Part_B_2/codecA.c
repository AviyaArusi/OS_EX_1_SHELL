#include <ctype.h>
#include "codecA.h"

void codecA_encode(char *str) {
    while (*str) {
        if (islower(*str)) {
            *str = toupper(*str);
        } else if (isupper(*str)) {
            *str = tolower(*str);
        }
        str++;
    }
}

void codecA_decode(char *str) {
    codecA_encode(str); // Since codecA is reversible, encode and decode are the same
}
