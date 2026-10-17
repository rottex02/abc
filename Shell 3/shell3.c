
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 1024

char PATH[1024] = "/usr/bin:/bin:/sbin";

int promptInput(char* str){
    char ch;
    int i = 0;

    while(i < SIZE-1 && read(0, &ch, 1) == 1){
        if(ch == '\n'){
            if(i == 0) return 2;
            break;
        }

        str[i] = ch;
        i++;
    }

    if(i == 0){
        write(1, "Exiting shell\n", 14);
        return 0;
    }

    str[i] = '\0';
    return 1;
}

void myexec(char* cmd, char* args[]){
    char temp[1024];
    strcpy(temp, PATH);

    char* dir = strtok(temp, ":");

    while(dir != NULL){
        char fullpath[1024];
        int i = 0;
        int j = 0;

        while(dir[i] != '\0'){
            fullpath[i] = dir[i];
            i++;
        }

        fullpath[i++] = '/';

        while(cmd[j] != '\0' && i < 1023){
            fullpath[i] = cmd[j];
            i++;
            j++;
        }

        fullpath[i] = '\0';

        execv(fullpath, args);

        dir = strtok(NULL, ":");
    }

    perror("command not found");
    exit(1);
}

int main(){
    char str[SIZE];

    while(1){
        write(1, "prompt>", 7);

        int pres = promptInput(str);

        if(pres == 0) break;
        if(pres == 2) continue;

        if(strcmp(str, "exit") == 0){
            write(1, "Exiting shell\n", 14);
            break;
        }

        char* commands[SIZE];
        int ncmd = 0;

        char* part = strtok(str, "|");

        while(part != NULL && ncmd < SIZE-1){
            commands[ncmd++] = part;
            part = strtok(NULL, "|");
        }

        commands[ncmd] = NULL;

        if(ncmd == 0) continue;

        int prevfd = -1;
        int pids[SIZE];
        int count = 0;
        int error = 0;

        for(int k = 0; k < ncmd; k++){
            char* args[100];
            int i = 0;

            char* word = strtok(commands[k], " \t");

            while(word != NULL && i < 98){
                args[i++] = word;
                word = strtok(NULL, " ");
            }

            args[i] = NULL;

            if(args[0] == NULL){
                error = 1;
                break;
            }

            if(ncmd == 1 && strcmp(args[0], "cd") == 0){
                char* path;
                if(args[1] == NULL){
                    path = getenv("HOME");
                }
                else{
                    path = args[1];
                }
                if(path == NULL || chdir(path) < 0){
                    perror("cd");
                }
                continue;
            }

            char* infile = NULL;
            char* outfile = NULL;

            for(int j = 0; args[j] != NULL; j++){
                if(strcmp(args[j], "<") == 0){
                    if(args[j+1] == NULL){
                        error = 1;
                        break;
                    }

                    infile = args[j+1];

                    for(int x = j; args[x+1] != NULL; x++){
                        args[x] = args[x+2];
                    }

                    j--;
                }
                else if(strcmp(args[j], ">") == 0){
                    if(args[j+1] == NULL){
                        error = 1;
                        break;
                    }

                    outfile = args[j+1];

                    for(int x = j; args[x+1] != NULL; x++){
                        args[x] = args[x+2];
                    }

                    j--;
                }
            }

            if(error) break;

            int pipefd[2] = {-1, -1};

            if(k < ncmd-1){
                if(pipe(pipefd) < 0){
                    perror("pipe");
                    error = 1;
                    break;
                }
            }

            int pid = fork();

            if(pid < 0){
                perror("fork");

                if(pipefd[0] != -1) close(pipefd[0]);
                if(pipefd[1] != -1) close(pipefd[1]);

                error = 1;
                break;
            }

            if(pid == 0){
                if(prevfd != -1){
                    if(dup2(prevfd, 0) < 0){
                        perror("dup2");
                        exit(1);
                    }
                }

                if(k < ncmd-1){
                    if(dup2(pipefd[1], 1) < 0){
                        perror("dup2");
                        exit(1);
                    }
                }

                if(prevfd != -1) close(prevfd);
                if(pipefd[0] != -1) close(pipefd[0]);
                if(pipefd[1] != -1) close(pipefd[1]);

                if(infile != NULL){
                    int fd = open(infile, O_RDONLY);

                    if(fd < 0){
                        perror("input file");
                        exit(1);
                    }

                    if(dup2(fd, 0) < 0){
                        perror("dup2");
                        close(fd);
                        exit(1);
                    }

                    close(fd);
                }

                if(outfile != NULL){
                    int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC,0644);

                    if(fd < 0){
                        perror("output file");
                        exit(1);
                    }

                    if(dup2(fd, 1) < 0){
                        perror("dup2");
                        close(fd);
                        exit(1);
                    }

                    close(fd);
                }

                myexec(args[0], args);
            }
            else{
                pids[count++] = pid;

                if(prevfd != -1){
                    close(prevfd);
                }

                if(pipefd[1] != -1){
                    close(pipefd[1]);
                }

                prevfd = pipefd[0];
            }
        }

        if(prevfd != -1){
            close(prevfd);
        }

        for(int k = 0; k < count; k++){
            waitpid(pids[k], NULL, 0);
        }
    }

    return 0;
}