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

int main()
{
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
            };
    num_pipes = 0;
    cmd_ptr = cmd;

    // parse commands separated by pipes
    while ((cmd_args[num_pipes][0] = strtok_r(cmd_ptr, "|", &cmd_ptr))) {
        num_pipes++;
    }

    for (i = 0; i < num_pipes; i++) {
        // parse individual command arguments
        char *arg;
        int arg_index = 0;

        while ((arg = strsep(&cmd_args[i][0], " ")) != NULL) {
            if (*arg != '\0') {
                cmd_args[i][arg_index] = arg;
                arg_index++;
            }
        }
        cmd_args[i][arg_index] = NULL;

        // create pipe
        if (i < num_pipes - 1) {
            if (pipe(pipe_fds[i]) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            output_fd = pipe_fds[i][1];
        } else {
            output_fd = STDOUT_FILENO;
        }

        // execute command
        execute_command(cmd_args[i][0], input_fd, output_fd);

        if (i < num_pipes - 1) {
            input_fd = pipe_fds[i][0];
            close(pipe_fds[i][1]);
        }
    }
}

return 0;

}