#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_CMD_LENGTH 1024
#define MAX_ARGS 64
#define MAX_PIPES 16

void handle_signal(int sig) {
    printf("\n");
}

void execute_command(char *command, int input_fd, int output_fd) {
    char *args[MAX_ARGS];
    char *arg;
    int arg_index = 0;
    int pipe_fds[2];
    int i;

    if (pipe(pipe_fds) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    while ((arg = strsep(&command, " ")) != NULL) {
        if (*arg != '\0') {
            args[arg_index] = arg;
            arg_index++;
        }
    }
    args[arg_index] = NULL;

    if (strcmp(args[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        if (input_fd != STDIN_FILENO) {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_fd != STDOUT_FILENO) {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
        if (output_fd == pipe_fds[1]) {
            close(pipe_fds[0]);
        }
        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        if (input_fd != STDIN_FILENO) {
            close(input_fd);
        }
        if (output_fd != STDOUT_FILENO) {
            close(output_fd);
        }
        if (output_fd == pipe_fds[1]) {
            close(pipe_fds[0]);
        }
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }

    if (command != NULL) {
        execute_command(command, pipe_fds[0], output_fd);
    }
}

int main() {
    char cmd[MAX_CMD_LENGTH];
    char *cmd_ptr;
    char *cmd_args[MAX_PIPES][MAX_ARGS];
    int num_pipes = 0;
    int pipe_fds[MAX_PIPES][2];
    int i, j;
    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;

    signal(SIGINT, handle_signal);

    while (1) {
        printf("stshell> ");
        fflush(stdout);

        if (fgets(cmd, MAX_CMD_LENGTH, stdin) == NULL) {
            if (feof(stdin)) {
                exit(EXIT_SUCCESS);
            } else {
                perror("fgets");
                exit(EXIT_FAILURE);
            }
        }

        if (strlen(cmd) > 0 && cmd[strlen(cmd) - 1] == '\n') {
            cmd[strlen(cmd) - 1] = '\0';
        }

        if (strcmp(cmd, "exit") == 0) {
            exit(EXIT_SUCCESS);
        }

        cmd_ptr = cmd;
        num_pipes = 0;

        while (1) {
            if (num_pipes > 0) {
                if (pipe(pipe_fds[num_pipes - 1]) == -1) {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
                output_fd = pipe_fds[num_pipes - 1][1];
            }

            j = 0;
            while ((cmd_args[num_pipes][j] = strsep(&cmd_ptr, " ")) != NULL) {
                if (*cmd_args[num_pipes][j] != '\0') {
                    j++;
                }
            }

            if (cmd_args[num_pipes][0] == NULL) {
                break;
            }

            num_pipes++;

            if (cmd_ptr == NULL) {
                break;
            }
        }

        for (i = 0; i < num_pipes; i++) {
            if (i == 0 && i == num_pipes - 1) {
                execute_command(*cmd_args, input_fd, output_fd);
            } else if (i == 0) {
                execute_command(*cmd_args, input_fd, pipe_fds[i][1]);
            } else if (i == num_pipes - 1) {
                execute_command(*cmd_args, pipe_fds[i - 1][0], output_fd);
            } else {
                execute_command(*cmd_args, pipe_fds[i - 1][0], pipe_fds[i][1]);
            }
        }

        for (i = 0; i < num_pipes - 1; i++) {
            close(pipe_fds[i][0]);
            close(pipe_fds[i][1]);
        }

        input_fd = STDIN_FILENO;
        output_fd = STDOUT_FILENO;
    }

    return 0;
}