#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAX_ARGS 10
#define MAX_COMMANDS 5

int keep_run = 1;

void sigint_handler(int sig) {
    printf("\n");
    fflush(stdout);
}

void run_command(char **argv, int input_fd, int output_fd, int is_last) {
    pid_t pid;
    int fd[2];
    int status;

    if (pipe(fd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        if (input_fd != STDIN_FILENO) {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        if (output_fd != STDOUT_FILENO) {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        if (is_last) {
            if (execvp(argv[0], argv) < 0) {
                perror("exec");
                exit(EXIT_FAILURE);
            }
        } else {
            if (dup2(fd[1], STDOUT_FILENO) < 0) {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(fd[0]);
            close(fd[1]);

            if (execvp(argv[0], argv) < 0) {
                perror("exec");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        close(fd[1]);
        if (input_fd != STDIN_FILENO) {
            close(input_fd);
        }
        if (!is_last) {
            run_command(argv+1, fd[0], output_fd, 0);
        }
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            exit(EXIT_FAILURE);
        }
    }
}

void parse_command(char *command, char **argv_list[MAX_COMMANDS], int *num_commands, int *output_append_flag) {
    char *token;
    char *saveptr;
    char *output_file = NULL;
    int i = 0;
    int j = 0;
    int argc = 0;

    // remove trailing newline character
    command[strlen(command) - 1] = '\0';

    // check for output redirection
    output_file = strstr(command, ">>");
    if (output_file) {
        *output_append_flag = 1;
    } else {
        output_file = strstr(command, ">");
    }
    if (output_file) {
        *output_file = '\0';
        output_file += 2;
        while (*output_file == ' ') {
            output_file++;
        }
    }

    // parse command line
    token = strtok_r(command, "|", &saveptr);
    while (token != NULL) {
        argv_list[i] = malloc(sizeof(char*) * (MAX_ARGS+1));
        argc = 0;
        argv_list[i][argc++] = strtok(token, " ");
        while ((argv_list[i][argc++] = strtok(NULL, " ")));
        argv_list[i][argc-1] = NULL;
        token = strtok_r(NULL, "|", &saveptr);
        i++;
    }
    argv_list[i] = NULL;
    *num_commands = i;
}

int main() {
    int i, j, k;
    char *argv[10];
    char command[1024];
    char *token;
    int pipe_pos[2] = {0, 0};
    int fd[2];
    int redirect_pos[2] = {0, 0};
    int output_fd;
    int append = 0;

    while (keep_run) {
        printf("stshell $ ");
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0'; // replace \n with \0

        // Check for empty command
        if (command[0] == '\0')
            continue;

        // Check for exit command
        if (strcmp(command, "exit") == 0) {
            printf("Exiting stshell...\n");
            keep_run = 0;
            break;
        }

        // Parse command line
        i = 0;
        token = strtok(command, " ");
        while (token != NULL) {
            // Check for pipe and redirection characters
            if (strcmp(token, "|") == 0) {
                pipe_pos[k++] = i;
            }
            else if (strcmp(token, ">") == 0) {
                redirect_pos[0] = i;
                append = 0;
            }
            else if (strcmp(token, ">>") == 0) {
                redirect_pos[0] = i;
                append = 1;
            }

            argv[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        argv[i] = NULL;

        // Check for pipes
        if (pipe_pos[0] > 0) {
            // Create pipes
            if (pipe(fd) == -1) {
                printf("Error creating pipes\n");
                break;
            }

            // Fork first child to execute first command
            if (fork() == 0) {
                close(fd[0]); // close read end of pipe
                dup2(fd[1], STDOUT_FILENO); // redirect stdout to write end of pipe
                close(fd[1]); // close write end of pipe

                // Execute first command
                argv[pipe_pos[0]] = NULL;
                execvp(argv[0], argv);
                printf("Error executing command\n");
                exit(1);
            }

            // Fork second child to execute second command
            if (fork() == 0) {
                close(fd[1]); // close write end of pipe
                dup2(fd[0], STDIN_FILENO); // redirect stdin to read end of pipe
                close(fd[0]); // close read end of pipe

                // Execute second command
                char **argv2 = &argv[pipe_pos[1] + 1]; // pointer to start of second command's arguments
                execvp(argv2[0], argv2);
                printf("Error executing command\n");
                exit(1);
            }

            // Parent process
            close(fd[0]); // close read end of pipe
            close(fd[1]); // close write end of pipe

            // Wait for children to finish
            wait(NULL);
            wait(NULL);
        }
        else {
            // Check for redirection
            if (redirect_pos[0] > 0) {
                // Open output file
                if (append)
                    output_fd = open(argv[redirect_pos[0]+1], O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
                else
                output_fd = open(argv[redirect_pos[0]+1], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
            
            if (output_fd == -1) {
                printf("Error opening file\n");
                break;
            }

            // Redirect stdout to file
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);

            // Remove redirection characters from command line
            argv[redirect_pos[0]] = NULL;
            if (redirect_pos[1] > 0)
                argv[redirect_pos[1]] = NULL;
        }

        // Execute command
        if (execvp(argv[0], argv) == -1) {
            printf("Error executing command\n");
            break;
        }
    }
}

return 0;

}
