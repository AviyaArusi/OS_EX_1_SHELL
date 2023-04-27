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

int arrow_flage = 0;

void run_command(char **argv)
{
    /* parse command line */
    int fd;
    char* line = argv[0];

    if (arrow_flage)
    {
        pid_t pid;

        // Fork a child process
        pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (strcmp(argv[1], ">") == 0)
        {
            if (pid == 0)
            {
                char* file_read =argv[0]; 
                char* file_to_write = argv[2]; 

                printf("we trying to do the copy from %s -> to %s \n", argv[0] , argv[2]);
                
                        // Execute command with or without redirection
                if (fork() == 0) {
                    if (type == 1) {
                        fd = open(file_to_write, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                    } 
                    else if (type == 2) 
                    {
                        fd = open(file_to_write, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
                        dup2(fd, STDOUT_FILENO);
                        close(fd);
                    }
                
                    execvp(argv[0], argv);
                    printf("Error: command not found\n");
                    exit(1);
                } 
                else {
                    wait(NULL);
                }
                
            }
            

        }

        if (strcmp(line, ">>") == 0)
        {
            printf("APPEND\n");
            // do here a append code use argv[0] , and argv[2] 
            return;
        }
    }
    
    if (strcmp(line, "exit") == 0)
    {
        printf("Going home...\n");
        keep_run = 0;
        return;
    }

    else if (strcmp(line, "|") == 0)
    {
        printf("let start the pip\n");

        // int *a;
        // fork();
        // int status = pipe(a);

    }
    /* Is command empty */
    else if (strcmp(line, "ls") == 0 || strcmp(line, "-l") == 0)
    {
        return;
    }

    else
    {
        printf("shell: %s: command not found\n" , line);
        return;
    }
}

void ignore_interrupt(int sig)
{
    // Do nothing
}

int main()
{
    int i;
    char *argv[10];
    char command[1024];
    char *token;

    while (keep_run)
    {
        // Ignore SIGINT signal
        signal(SIGINT, ignore_interrupt);

        printf("Ron&Aviya → shell $ ");
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0'; // replace \n with \0
        
            // check if we need to do the file option
        char* result;
        result = strstr(command, ">");
        if(result != NULL) {
            arrow_flage = 1;
            type = 1;
        }

        result = strstr(command, ">>");
        if(result != NULL) {
            arrow_flage = 1;
            type = 2;
        }

        /* parse command line */
        i = 0;
        token = strtok(command, " ");
        while (token != NULL)
        {
            argv[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        argv[i] = NULL;

        

        /* Is command empty */
        if (argv[0] == NULL)
            continue;

        run_command(argv);

        /* for commands not part of the shell command language */
        if (strcmp(argv[0], "ls") == 0){
            if (fork() == 0)
            {
                execvp(argv[0], argv);
                perror("execvp");
                exit(EXIT_FAILURE);
            }
            else
            {
                wait(NULL);
            }
        }
    }
}