#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <codec> <message>\n", argv[0]);
        return 1;
    }

    char *codec = argv[1];
    char *message = argv[2];

    char *decoded = decode(codec, message);

    printf("%s\n", decoded);

    free(decoded);

    return 0;
}
