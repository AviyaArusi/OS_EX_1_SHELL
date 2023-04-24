#include "codecB.h"

void encode_codecB(char* message) {
    for(int i = 0; message[i]; i++) {
        message[i] = message[i] + 3;
    }
}

void decode_codecB(char* message) {
    for(int i = 0; message[i]; i++) {
        message[i] = message[i] - 3;
    }
}
