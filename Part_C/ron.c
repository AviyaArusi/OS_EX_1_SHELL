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

int pipe_flage = 0;

void run_command(char **argv)
{
    /* parse command line */

    char* line = argv[0];

    pid_t pid;
    
    if (arrow_flage)
    {
        pid = fork();

        if (pid == 0)
        {

            char* file_read =argv[0];
            char* file_to_write = argv[2]; 
            int fd;

            printf("we trying to do the copy from %s -> to %s \n", argv[0] , argv[2]);
        
            // Execute command with or without redirection
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
            perror("execvp");
            exit(1);
        }
        else 
        {
            wait(NULL);
            
        }
    }

    else if (pipe_flage)
    {
        pipe_flage = 0;
        printf("let start the pip\n");
        // int fd[2];

        // if (pipe(fd) == -1) {
        //     perror("pipe");
        //     exit(EXIT_FAILURE);
        // }

        // pid = fork();
        // if (pid == -1) {
        //     perror("fork");
        //     exit(EXIT_FAILURE);
        // }
        // else if (pid == 0) {
        //     close(fd[0]);
        //     if (dup2(fd[1], STDOUT_FILENO) == -1) {
        //         perror("dup2");
        //         exit(EXIT_FAILURE);
        //     }
        //     close(fd[1]);
        //     execvp(argv[0], argv);
        //     perror("execvp");
        //     exit(EXIT_FAILURE);
        // }
        // else {
        //     wait(NULL);
        //     pid = fork();
        //     if (pid == -1) {
        //         perror("fork");
        //         exit(EXIT_FAILURE);
        //     }
        //     else if (pid == 0) {
        //         close(fd[1]);
        //         if (dup2(fd[0], STDIN_FILENO) == -1) {
        //             perror("dup2");
        //             exit(EXIT_FAILURE);
        //         }
        //         close(fd[0]);
        //         char* cmd2_argv[] = {argv[2], NULL};
        //         execvp(argv[2], cmd2_argv);
        //         perror("execvp");
        //         exit(EXIT_FAILURE);
        //     }
        //     else {
        //         close(fd[0]);
        //         close(fd[1]);
        //         wait(NULL);
        //     }
        // }
        pid_t pid1, pid2; 
        int pipefd[2]; 
 
   // The two commands we'll execute.  In this simple example, we will pipe 
   // the output of `ls` into `wc`, and count the number of lines present. 
   char *argv1[] = {"ls", "-l", "-h", NULL}; 
   char *argv2[] = {"wc", "-l", NULL}; 
 
   // Create a pipe. 
   pipe(pipefd); 
 
   // Create our first process. 
   pid1 = fork(); 
   if (pid1 == 0) { 
      // Hook stdout up to the write end of the pipe and close the read end of 
      // the pipe which is no longer needed by this process. 
      dup2(pipefd[1], STDOUT_FILENO); 
      close(pipefd[0]); 
 
      // Exec `ls -l -h`.  If the exec fails, notify the user and exit.  Note 
      // that the execvp variant first searches the $PATH before calling execve. 
      execvp(argv[0], argv); 
      perror("exec"); 
      return; 
   } 
 
   // Create our second process. 
   pid2 = fork(); 
   if (pid2 == 0) { 
      // Hook stdin up to the read end of the pipe and close the write end of 
      // the pipe which is no longer needed by this process. 
      dup2(pipefd[0], STDIN_FILENO); 
      close(pipefd[1]); 
 
      // Similarly, exec `wc -l`. 
      execvp(argv[0], argv); 
      perror("exec"); 
      return; 
   } 
 
   // Close both ends of the pipe.  The respective read/write ends of the pipe 
   // persist in the two processes created above (and happen to be tying stdout 
   // of the first processes to stdin of the second). 
   close(pipefd[0]); 
   close(pipefd[1]); 
 
   // Wait for everything to finish and exit. 
   int status;
   waitpid(pid1 , &status , 0); 
   waitpid(pid2 , &status , 0); 
   return; 
        printf("here\n");
    }
    
    else {
        if (strcmp(line, "exit") == 0)
        {
            printf("Going home...\n");
            keep_run = 0;
            return;
        }
    
        else if (strcmp(line, "ls") == 0 )
        {
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

        else
        {
            printf("shell: %s: command not found\n" , line);
            return;
        }
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

        printf("\033[1;32m@Ron&Aviya\033[0m → \033[1;34m/shell\033[0m (\033[1;31mmain\033[0m) $ ");
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0'; // replace \n with \0
        
        // Split command into arguments
        int num_argv = 0;
        argv[num_argv] = strtok(command, " ");
        while (argv[num_argv] != NULL) {
            num_argv++;
            argv[num_argv] = strtok(NULL, " ");
        }

        for (int i = 0; i < num_argv; i++) {
            if (strcmp(argv[i], ">") == 0) {
                type = 1;
                arrow_flage = 1;
                argv[i] = NULL;
            } else if (strcmp(argv[i], ">>") == 0) {
                type = 2;
                arrow_flage = 1;
                argv[i] = NULL;
            }
            else if (strcmp(argv[i] , "|") == 0)
            {
                pipe_flage = 1;
                argv[i] = NULL;
            }
            
        }

        /* Is command empty */
        if (argv[0] == NULL)
            continue;

        run_command(argv);

        arrow_flage = 0;

    }
}