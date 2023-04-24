#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void usage() {
    printf("Usage: cmp <file1> <file2> [-v] \n");
    exit(1);
}

int cmp_files(char* filename1, char* filename2, int ignore_case) {
    FILE* f1 = fopen(filename1, "r");
    FILE* f2 = fopen(filename2, "r");
    if (f1 == NULL || f2 == NULL) {
        perror("Error opening file");
        exit(1);
    }
    int equal = 1;
    int c1, c2;
    while ((c1 = fgetc(f1)) != EOF && (c2 = fgetc(f2)) != EOF) { 
        if (ignore_case) {
            c1 = tolower(c1);
            c2 = tolower(c2);
        }
        if (c1 != c2) {
            equal = 0;
            break;
        }
    }
    if (equal && (fgetc(f2) != EOF) ) { // ron --- ron1
        equal = 0;
    }
    if (equal && c1 != EOF ) { // ron1 --- ron
        equal = 0;
    }
    // close the files
    fclose(f1);
    fclose(f2);

    return equal;
}

int main(int argc, char* argv[]) {
    int verbose = 0;
    int ignore_case = 0;
    char* filename1;
    char* filename2;
    if (argc < 3) {
        usage();
    }
    filename1 = argv[1];
    filename2 = argv[2];
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } 
        if (strcmp(argv[i], "-i") == 0) {
            ignore_case = 1;
        } 
    }
    
    int ans = cmp_files(filename1, filename2, ignore_case);
    //printf( "The answer is : %d \nThe verbose is : %d \n" , ans , verbose);
    if (verbose)
    {
        printf(ans ? "equal\n" : "distinct\n");
    }

    return !ans;

}

//gcc -o cmp cmp.c
//./cmp file1 file2 -v -i