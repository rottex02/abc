#include <stdio.h>
#include <unistd.h>
int main() {
	printf("bye\n");
	execl("/usr/bin/ls", "ls", "-l", NULL);
	printf("hi\n");
}
