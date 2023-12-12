#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Executes the command "cat scores | grep <argument> | sort". In this
 * implementation, the parent waits for both child processes to finish.
 *
 */

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <search_term>\n", argv[0]);
        return 1;
    }

    int pipefd1[2];
    int pipefd2[2];
    int pid1, pid2;

    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    // create the first pipe (for communication between cat and grep)
    if (pipe(pipefd1) == -1)
    {
        perror("pipe");
        return 1;
    }

    // create the second pipe (for communication between grep and sort)
    if (pipe(pipefd2) == -1)
    {
        perror("pipe");
        return 1;
    }

    // fork the first child process (P1)
    pid1 = fork();

    if (pid1 == 0)
    {
        // child process P1 (execute "cat scores")

        // replace standard output with output part of the first pipe
        dup2(pipefd1[1], 1);

        // close unused input half of the first pipe
        close(pipefd1[0]);

        // close unused half of the second pipe
        close(pipefd2[0]);
        close(pipefd2[1]);

        // execute cat
        execvp("cat", cat_args);
    }
    else
    {
        // parent process

        // fork the second child process (P2)
        pid2 = fork();

        if (pid2 == 0)
        {
            // child process P2 (execute "grep <argument>")

            // replace standard input with input part of the first pipe
            dup2(pipefd1[0], 0);

            // replace standard output with output part of the second pipe
            dup2(pipefd2[1], 1);

            // close unused halves of both pipes
            close(pipefd1[1]);
            close(pipefd2[0]);

            // execute grep
            execvp("grep", grep_args);
        }
        else
        {
            // parent process

            // close unused halves of both pipes
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[1]);

            // replace standard input with input part of the second pipe
            dup2(pipefd2[0], 0);

            // execute sort
            execvp("sort", sort_args);

            // parent waits for both child processes to finish
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    }

    return 0;
}