#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int fd;
	char str[16];
	close(0);
	fd = open(argv[1], O_RDONLY);
	scanf("%s", str);
	printf("%s\n", str);
	return 0;
}
