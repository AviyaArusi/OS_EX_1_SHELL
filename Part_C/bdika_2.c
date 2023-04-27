#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 1024

int main() {
    char command[MAX_COMMAND_LENGTH];
    char* args[64];
    int num_args;
    int redirect_type;
    char* redirect_file;
    int fd;

    while (1) {
        printf("stshell> ");
        fgets(command, MAX_COMMAND_LENGTH, stdin);

        // Remove newline character from command
        command[strcspn(command, "\n")] = 0;

        // Split command into arguments
        num_args = 0;
        args[num_args] = strtok(command, " ");
        while (args[num_args] != NULL) {
            num_args++;
            args[num_args] = strtok(NULL, " ");
        }

        // Check if command has a redirect symbol
        redirect_type = 0;
        redirect_file = NULL;
        for (int i = 0; i < num_args; i++) {
            if (strcmp(args[i], ">") == 0) {
                redirect_type = 1;
                redirect_file = args[i+1];
                args[i] = NULL;
            } else if (strcmp(args[i], ">>") == 0) {
                redirect_type = 2;
                redirect_file = args[i+1];
                args[i] = NULL;
            }
        }

        // Execute command with or without redirection
        if (fork() == 0) {
            if (redirect_type == 1) {
                fd = open(redirect_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            } else if (redirect_type == 2) {
                fd = open(redirect_file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            execvp(args[0], args);
            printf("Error: command not found\n");
            exit(1);
        } else {
            wait(NULL);
        }
    }

    return 0;
}
