#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
	int pid;
	char buf[128];
	while(1){
		printf("prompt>");
		scanf("%s", buf);
		if(strcmp(buf, "exit") == 0)
			return 0;	
		pid = fork();
		if(pid == 0) {
			execl(buf, buf, NULL);
		} else {
			wait(0);
		}
	}
}
