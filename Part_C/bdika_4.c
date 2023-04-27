#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

// global parameters
int keep_run = 1;

int type = 0;

int arrow_flag = 0;

void run_command(char **argv, int *fd_in)
{
    /* parse command line */
    char* line = argv[0];
    pid_t pid;
    int fd[2];

    if (strcmp(line, "exit") == 0)
    {
        printf("Going home...\n");
        keep_run = 0;
        return;
    }

    if (pipe(fd) == -1) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        if (*fd_in != STDIN_FILENO) 
        {
            if (dup2(*fd_in, STDIN_FILENO) == -1)
            {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(*fd_in);
        }

        if (fd[1] != STDOUT_FILENO)
        {
            if (dup2(fd[1], STDOUT_FILENO) == -1)
            {
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(fd[1]);
        }

        if (strcmp(line, "ls") == 0 || strcmp(line, "-l") == 0)
        {
            if (execvp("ls", argv) == -1)
            {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if (execvp(argv[0], argv) == -1)
            {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        close(fd[1]);

        if (*fd_in != STDIN_FILENO)
        {
            close(*fd_in);
        }

        if (strcmp(argv[0], "exit") == 0)
        {
            waitpid(pid, NULL, 0);
            keep_run = 0;
            return;
        }

        if (strcmp(argv[0], "|") == 0)
        {
            int new_fd_in = fd[0];
            run_command(&argv[1], &new_fd_in);
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);

            if (arrow_flag)
            {
                arrow_flag = 0;
                return;
            }

            if (status != 0)
            {
                printf("shell: %s: command not found\n", line);
                return;
            }
        }
    }
}

void ignore_interrupt(int sig)
{
    // Do nothing
}

int main()
{
    signal(SIGINT, ignore_interrupt);
    char* line = NULL;
    size_t line_size = 0;

    while (keep_run)
    {
        printf("$ ");

        if (getline(&line, &line_size, stdin) == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("getline");
                exit(EXIT_FAILURE);
            }
        }

        // split arguments
        char* arg;
        char** args = malloc(sizeof(char*) * 128);
        int arg_count = 0;

        arg = strtok(line, " \n");
        while (arg != NULL)
        {
            if (strcmp(arg, ">") == 0)
            {
                type = 1;
                arg = strtok(NULL, " \n");
                if (arg == NULL)
                {
                    printf("shell: syntax error near unexpected token `newline'\n");
                    break;
                }
                int fd = open(arg, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
                if (fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
                arrow_flag = 1;
            }
            else if (strcmp(arg, "<") == 0)
            {
                type = 2;
                arg = strtok(NULL, " \n");
                if (arg == NULL)
                {
                    printf("shell: syntax error near unexpected token `newline'\n");
                    break;
                }
                int fd = open(arg, O_RDONLY);
                if (fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
                arrow_flag = 1;
            }
            else if (strcmp(arg, "|") == 0)
            {
                type = 3;
            }
            else
            {
                args[arg_count++] = arg;
            }

            arg = strtok(NULL, " \n");
        }
        args[arg_count] = NULL;

        if (arg_count > 0)
        {
            int fd_in = STDIN_FILENO;
            run_command(args, &fd_in);
        }

        if (type == 1)
        {
            dup2(STDOUT_FILENO, STDOUT_FILENO);
            type = 0;
        }
        else if (type == 2)
        {
            dup2(STDIN_FILENO, STDIN_FILENO);
            type = 0;
        }
        else if (type == 3)
        {
            type = 0;
        }

        free(args);
    }

    free(line);
    return 0;
}