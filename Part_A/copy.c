#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void usage() {
    printf("Usage: copy <source> <destination> [-v] [-f]\n");
    exit(1);
}

bool file_exists(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return false;
    }
    fclose(file);
    return true;
}

int copy_file(char* source, char* destination, bool verbose, bool force) {
    if (!force && file_exists(destination)) {
        if (verbose) {
            printf("target file exists\n");
        }
        return 1;
    }
    FILE* src_file = fopen(source, "r");
    if (src_file == NULL) {
        perror("Error opening source file");
        exit(1);
    }
    FILE* dest_file = fopen(destination, "w");
    if (dest_file == NULL) {
        perror("Error opening destination file");
        exit(1);
    }
    int c;
    while ((c = fgetc(src_file)) != EOF) {
        fputc(c, dest_file);
    }
    fclose(src_file);
    fclose(dest_file);
    if (verbose) {
        printf("success\n");
    }
    return 0;
}

int main(int argc, char* argv[]) {
    bool verbose = false;
    bool force = false;
    char* source;
    char* destination;
    if (argc < 3) {
        usage();
    }
    source = argv[1];
    destination = argv[2];
    int i;
    for (i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-f") == 0) {
            force = true;
        } else {
            usage();
        }
    }
    return copy_file(source, destination, verbose, force);
}

//gcc -o copy copy.c
//./copy file1 file2 -v -f
