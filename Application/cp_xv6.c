#include "types.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]) {
    int fd1;
    int fd2;
    int n;
    char line[512];

    if(argc != 3) {
        printf(1, "Usage: cp source destination\n");
        exit();
    }

    fd1 = open(argv[1], O_RDONLY);

    if(fd1 < 0) {
        printf(1, "Cannot open file %s\n", argv[1]);
        exit();
    }

    fd2 = open(argv[2], O_WRONLY | O_CREATE);

    if(fd2 < 0) {
        printf(1, "Cannot create file %s\n", argv[2]);
        close(fd1);
        exit();
    }

    n = read(fd1, line, sizeof(line));

    while(n > 0) {

        if(write(fd2, line, n) != n) {
            printf(1, "Cannot open file\n");
            exit();
        }

        n = read(fd1, line, sizeof(line));
    }

    close(fd1);
    close(fd2);

    exit();
}