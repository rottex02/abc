#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 1024

int promptInput(char* str){
	char ch;
	int i = 0;
	while(i < SIZE-1 && read(0, &ch, 1) == 1){
		if(ch == '\n'){
			if(i == 0) return 2;
			break;
		}
		str[i] = ch;
		i++;
	}
	
	if(i == 0){
		write(1,"Exiting shell\n",14);
		return 0;
	}
	str[i] = '\0';
	return 1;
}


int main(){
	char str[1024];
	while(1){
		write(1,"prompt>",7);
		
		int pres = promptInput(str);
		if(pres == 0) break;
		else if(pres == 2) continue;
		
		char* args[100];
		
		args[0] = strtok(str, " ");
		int i = 1;
		
		while(i < 100 && args[i-1] != NULL){
			args[i] = strtok(NULL, " ");
			i++;
		}
		
		if(strcmp(args[0], "cd") == 0){
			char* path;
			if(args[1] == NULL){
				path = getenv("HOME");
			}else{
				path = args[1];
			}
			
			int res = chdir(path);
			if(res < 0) perror("cd");
			continue;
		}
		
		
		int pid = fork();
		if(pid == 0){
			execvp(args[0], args);
			return -1;
		}
		else{
			wait(NULL);
		}	
	}
	return 0;
}
