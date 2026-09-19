#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int fd;
	char str[16];
	close(1);
	fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC);
	if(fd == -1) {
		perror("open failed:");
		exit(errno);
	}
	scanf("%s", str);
	printf("%s\n", str);
	return 0;
}
