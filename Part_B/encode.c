#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

// Function pointer type for the encoding and decoding functions
typedef char* (*CodecFunc)(const char*);

int main(int argc, char* argv[]) {
    // Check that the correct number of arguments were provided
    if (argc != 3) {
        printf("Usage: encode <codec> <message>\n");
        return 1;
    }

    // Load the shared library based on the selected codec
    void* libHandle = dlopen(argv[1], RTLD_LAZY);
    if (!libHandle) {
        printf("Error: Failed to load library %s\n", argv[1]);
        return 1;
    }

    // Get pointers to the encoding and decoding functions
    CodecFunc encodeFunc = (CodecFunc)dlsym(libHandle, "encode");
    CodecFunc decodeFunc = (CodecFunc)dlsym(libHandle, "decode");
    char* error = dlerror();
    if (error) {
        printf("Error: %s\n", error);
        dlclose(libHandle);
        return 1;
    }

    // Call the appropriate function based on the command line arguments
    char* input = argv[2];
    if (strcmp(input, "-h") == 0 || strcmp(input, "--help") == 0) {
        printf("Usage: encode <codec> <message>\n");
        printf("Encodes or decodes a message using the specified codec.\n\n");
        printf("Available codecs:\n");
        printf(" - codecA\n");
        printf(" - codecB\n");
        dlclose(libHandle);
        return 0;
    } else {
        char* output;
        if (strcmp(argv[1], "codecA") == 0) {
            output = encodeFunc(input);
        } else if (strcmp(argv[1], "codecB") == 0) {
            output = decodeFunc(input);
        } else {
            printf("Error: Invalid codec %s\n", argv[1]);
            dlclose(libHandle);
            return 1;
        }
        printf("%s\n", output);
        free(output);
    }

    // Clean up and exit
    dlclose(libHandle);
    return 0;
}
