#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int fd;
	
	if (argc < 3) {
		printf("Valid format : %s <word to search> <filename>\n", argv[0]);
		return 1;
	}
	
	fd = open(argv[1], O_RDONLY);
	
	if (fd == -1) {
		printf("open failed : %s\n", argv[1]);
		return 1;
	}
	
	char line[100], ch;
	char *word = argv[2];
	
	while (1) {
		int i = 0;
		int n;
		while (i < 99 && (n = read(fd, &ch, 1)) > 0 && ch != '\n') {
			line[i] = ch;
			i++;
		}
		
		if (n == 0 && i == 0)
			break;
		
		line[i] = '\0';
	
		if (strstr(line, word) != NULL) {
			write(1, line, strlen(line));
			printf("\n");
			//printf("%s\n", line);
		}
	}
	
	close(fd);
	return 0;
}
