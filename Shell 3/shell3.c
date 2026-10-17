
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 1024

int main()
{
    char str[SIZE];

    while (1)
    {
        write(1, "prompt>", 7);

        int len = 0;
        char ch;

        while (len < SIZE - 1 && read(0, &ch, 1) == 1)
        {
            if (ch == '\n')
                break;

            str[len++] = ch;
        }

        if (len == 0)
        {
            write(1, "Exiting shell\n", 14);
            break;
        }

        str[len] = '\0';

        if (strcmp(str, "exit") == 0)
        {
            write(1, "Exiting shell\n", 14);
            break;
        }

        char *commands[100];
        int ncmd = 0;

        commands[ncmd] = strtok(str, "|");

        while (commands[ncmd] != NULL && ncmd < 99)
        {
            ncmd++;
            commands[ncmd] = strtok(NULL, "|");
        }

        char *firstarg = strtok(commands[0], " \t");

        if (ncmd == 1 && firstarg != NULL &&
            strcmp(firstarg, "cd") == 0)
        {
            char *path = strtok(NULL, " \t");

            if (path == NULL)
                path = getenv("HOME");

            if (path != NULL && chdir(path) < 0)
                perror("cd");

            continue;
        }

        int prevfd = -1;

        for (int k = 0; k < ncmd; k++)
        {
            char *args[100];
            int i = 0;

            args[i] = strtok(commands[k], " \t");

            while (args[i] != NULL && i < 98)
            {
                i++;
                args[i] = strtok(NULL, " \t");
            }

            args[i] = NULL;

            if (args[0] == NULL)
                continue;

            char *infile = NULL;
            char *outfile = NULL;

            for (int j = 0; args[j] != NULL; j++)
            {
                if (strcmp(args[j], "<") == 0)
                {
                    infile = args[j + 1];
                    args[j] = NULL;
                    break;
                }

                if (strcmp(args[j], ">") == 0)
                {
                    outfile = args[j + 1];
                    args[j] = NULL;
                    break;
                }
            }

            int pipefd[2];

            if (k < ncmd - 1)
            {
                if (pipe(pipefd) < 0)
                {
                    perror("pipe");
                    break;
                }
            }

            int pid = fork();

            if (pid < 0)
            {
                perror("fork");
                break;
            }

            if (pid == 0)
            {
                if (prevfd != -1)
                {
                    dup2(prevfd, 0);
                    close(prevfd);
                }

                if (k < ncmd - 1)
                {
                    dup2(pipefd[1], 1);
                    close(pipefd[0]);
                    close(pipefd[1]);
                }

                if (infile != NULL)
                {
                    int fd = open(infile, O_RDONLY);

                    if (fd < 0)
                    {
                        perror("input file");
                        exit(1);
                    }

                    dup2(fd, 0);
                    close(fd);
                }

                if (outfile != NULL)
                {
                    int fd = open(outfile,
                                   O_WRONLY | O_CREAT | O_TRUNC,
                                   0644);

                    if (fd < 0)
                    {
                        perror("output file");
                        exit(1);
                    }

                    dup2(fd, 1);
                    close(fd);
                }

                execvp(args[0], args);

                perror("command not found");
                exit(1);
            }
            else
            {
                if (prevfd != -1)
                    close(prevfd);

                if (k < ncmd - 1)
                {
                    close(pipefd[1]);
                    prevfd = pipefd[0];
                }
            }
        }

        if (prevfd != -1)
            close(prevfd);

        for (int k = 0; k < ncmd; k++)
            wait(NULL);
    }

    return 0;
}
