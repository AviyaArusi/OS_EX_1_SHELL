#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_COMMANDS 10
#define MAX_ARGUMENTS 10
#define MAX_BUFFER_SIZE 1024

// Parse a command string into a list of commands and arguments
void parse_command(char* command, char** commands, char*** arguments, int* num_commands) {
    char* token;
    char* rest = command;
    int i = 0;

    // Parse the first command
    token = strtok_r(rest, "|", &rest);
    commands[i] = token;
    arguments[i][0] = token;

    // Parse the rest of the commands and arguments
    while ((token = strtok_r(rest, "|", &rest))) {
        i++;
        commands[i] = token;

        // Parse the arguments for this command
        int j = 0;
        char* arg_token = strtok(token, " ");
        while (arg_token) {
            arguments[i][j++] = arg_token;
            arg_token = strtok(NULL, " ");
        }
        arguments[i][j] = NULL;
    }

    *num_commands = i + 1;
}

// Execute a command with arguments and input/output redirection
void execute_command(char** arguments, char* input_file, char* output_file, int append_output) {
    int input_fd = 0;
    int output_fd = 1;

    // Redirect input if specified
    if (input_file != NULL) {
        input_fd = open(input_file, O_RDONLY);
        if (input_fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        if (dup2(input_fd, STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
    }

    // Redirect output if specified
    if (output_file != NULL) {
        int flags = O_WRONLY | O_CREAT;
        if (append_output) {
            flags |= O_APPEND;
        } else {
            flags |= O_TRUNC;
        }
        output_fd = open(output_file, flags, 0666);
        if (output_fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        if (dup2(output_fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
    }

    // Execute the command
    execvp(arguments[0], arguments);
    perror("execvp");
    exit(EXIT_FAILURE);
}

int main() {
    char buffer[MAX_BUFFER_SIZE];
    char* commands[MAX_COMMANDS];
    char*** arguments[MAX_COMMANDS][MAX_ARGUMENTS];
    int num_commands = 0;

    while (1) {
        // Print the shell prompt
        printf("stshell> ");

        // Read a command from the user
        if (fgets(buffer, MAX_BUFFER_SIZE, stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove the newline character from the end of the command
        buffer[strcspn(buffer, "\n")] = '\0';

        // Split the command into a list of commands and arguments
        char* commands[MAX_COMMANDS];
        char*** arguments[MAX_COMMANDS][MAX_ARGUMENTS];
        int num_commands = 0;
        parse_command(buffer, commands, arguments, &num_commands);

        // Execute the commands
        int input_fd = 0;
        int i;
        for (i = 0; i < num_commands; i++) {
            int output_fd = 1;
            int append_output = 0;

            // Check for output redirection
            char* output_file = NULL;
            char* append_token = strstr(commands[i], ">>");
            if (append_token) {
                append_output = 1;
                output_file = append_token + 2;
                *append_token = '\0';
            } else {
                char* output_token = strstr(commands[i], ">");
                if (output_token) {
                    output_file = output_token + 1;
                    *output_token = '\0';
                }
            }

            // Check for input redirection
            char* input_file = NULL;
            char* input_token = strstr(arguments[i][0], "<");
            if (input_token) {
                input_file = input_token + 1;
                *input_token = '\0';
            }

            // Execute the command with arguments and redirection
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                // Child process
                if (i < num_commands - 1) {
                    // Not the last command, redirect output to pipe
                    int pipe_fd[2];
                    if (pipe(pipe_fd) == -1) {
                        perror("pipe");
                        exit(EXIT_FAILURE);
                    }
                    output_fd = pipe_fd[1];
                    if (dup2(output_fd, STDOUT_FILENO) == -1) {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                    close(pipe_fd[0]);
                }
                if (i > 0) {
                    // Not the first command, redirect input from pipe
                    if (dup2(input_fd, STDIN_FILENO) == -1) {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                    close(input_fd);
                }
                execute_command(arguments[i], input_file, output_file, append_output);
            } else {
                // Parent process
                if (i > 0) {
                    // Not the first command, close input end of previous pipe
                    close(input_fd);
                }
                input_fd = output_fd;
                close(output_fd);
                wait(NULL);
            }
        }
    }

    return 0;
}