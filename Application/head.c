#include "types.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]) {
    int file = 0;
    int count = 10;
    int readBytes;
    int lines = 0;
    char data[512];
    char *name = 0;

    if (argc == 1) {
        file = 0;
    }
    else if (argc == 2) {
        if (argv[1][0] != '-') {
            name = argv[1];
        }
    }
    else if (argv[1][0] == '-' && argv[1][1] == 'n') {
        count = atoi(argv[2]);

        if (argc == 3) {
            file = 0;
        }
        else if (argc == 4) {
            name = argv[3];
        }
    }
    else {
        printf(1, "Incorrect usage\n");
        exit();
    }

    if (name != 0) {
        file = open(name, O_RDONLY);

        if (file < 0) {
            printf(1, "Error: cannot open file %s\n", name);
            exit();
        }
    }

    while (lines < count) {
        readBytes = read(file, data, 1);

        if (readBytes <= 0)
            break;

        write(1, data, 1);

        if (data[0] == '\n') {
            lines++;
        }
    }

    if (name != 0) {
        close(file);
    }

    exit();
}
