#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[]) {
    int fd1;
    int fd2;
    char ch;

    if(argc != 3) {
        printf("Usage: cp source destination\n");
        return 1;
    }

    fd1 = open(argv[1], O_RDONLY);

    if(fd1 < 0) {
        printf("cannot open source file\n");
        return 1;
    }

    fd2 = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if(fd2 < 0) {
        printf("cp: cannot create destination file\n");
        close(fd1);
        return 1;
    }

    while(read(fd1, &ch, 1) == 1) {
        write(fd2, &ch, 1);
    }

    close(fd1);
    close(fd2);

    return 0;
}