#include "types.h"
#include "user.h"
#include "fcntl.h"

char buf[10000];

int main(int argc, char *argv[]) {
    int fd;
    int n;
    int nlines = 10;
    char *filename = 0;

    if(argc == 1){
        fd = 0;
    } else if(argc == 2 && argv[1][0] != '-'){
        filename = argv[1];
    } else if(argc == 3 && argv[1][0] == '-' && argv[1][1] == 'n'){
        nlines = atoi(argv[2]);
        fd = 0;
    } else if(argc == 4 && argv[1][0] == '-' && argv[1][1] == 'n'){
        nlines = atoi(argv[2]);
        filename = argv[3];
    } else {
        printf(1, "Incorrect syntax\n");
        exit();
    }

    if(filename){
        fd = open(filename, O_RDONLY);
        if(fd < 0){
            printf(1, "Error: cannot open file %s\n", filename);
            exit();
        }
    }

    // Read complete input in buf
    int total = 0;
    while((n = read(fd, buf + total, 1)) > 0){
        total++;
        if(total >= sizeof(buf) - 1)
            break;
    }
    buf[total] = 0;

    if(filename)
        close(fd);


    int totallines = 0;
    for(int i = 0; i < total; i++){
        if(buf[i] == '\n')
            totallines++;
    }


    int linesToSkip = totallines - nlines; //number of \n i need to skip
    if(linesToSkip < 0)
        linesToSkip = 0; //as if total lines are less than the given 'n' , i need to write all lines in the output


    int currline = 0;
    int i = 0;
    while(currline < linesToSkip && i < total){
        if(buf[i] == '\n')
            currline++;
        i++;
    }

    write(1, buf + i, total - i);

    exit();
}
