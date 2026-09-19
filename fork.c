#include <stdio.h>
#include <unistd.h>
int main() {
	int pid;
	printf("bye\n");
	pid = fork();
	if(pid == 0) {
		printf("in child\n");
	} else {
		printf("in parent, child'd pid is %d\n", pid);
	}
	printf("hi\n");
	getchar();
}
