#include <ctype.h>
#include "codecA.h"

void encode_codecA(char* message) {
    for(int i = 0; message[i]; i++) {
        if(islower(message[i])) {
            message[i] = toupper(message[i]);
        }
        else if(isupper(message[i])) {
            message[i] = tolower(message[i]);
        }
    }
}

void decode_codecA(char* message) {
    for(int i = 0; message[i]; i++) {
        if(islower(message[i])) {
            message[i] = toupper(message[i]);
        }
        else if(isupper(message[i])) {
            message[i] = tolower(message[i]);
        }
    }
}
