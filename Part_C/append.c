#include <stdio.h>


bool file_exists(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return false;
    }
    fclose(file);
    return true;
}

void appendToFile(char* str, char* fileName) {
    if (!file_exists(fileName))
    {
        printf("The file does not exist!\n")
        return;
    }
    
    // Open the file in append mode
    FILE* file = fopen(fileName, "a");
    
    // Check if the file was opened successfully
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    
    // Append the string to the end of the file
    fprintf(file, "%s", str);
    
    // Close the file
    fclose(file);
}


int main() {
    char str[] = "Hello, world!\n";
    char fileName[] = "output.txt";
    appendToFile(str, fileName);
    return 0;
}
