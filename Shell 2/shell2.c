#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>

#define SIZE 1024

char PATH[1024] = "/usr/bin:/bin:/sbin";

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

void myexec(char* cmd, char* args[]){
	char temp[1024];
	strcpy(temp, PATH);

	char* dir = strtok(temp, ":");

	while(dir != NULL){

		char fullpath[1024];
		int i = 0;
		int j = 0;

		while(dir[i] != '\0'){
			fullpath[i] = dir[i];
			i++;
		}

		fullpath[i++] = '/';

		while(cmd[j] != '\0'){
			fullpath[i] = cmd[j];
			i++;
			j++;
		}

		fullpath[i] = '\0';
		execv(fullpath, args);
		dir = strtok(NULL, ":");
	}

	perror("command not found");
	exit(1);
}


int main(){
	char str[1024];

	while(1){
		write(1,"prompt>",7);
		int pres = promptInput(str);

		if(pres == 0) break;
		else if(pres == 2) continue;

		if(strcmp(str, "exit") == 0){
			write(1,"Exiting shell\n",14);
			break;
		}

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
			}
			else{
				path = args[1];
			}
			chdir(path);
			continue;
		}

		char* infile = NULL;
		char* outfile = NULL;

		int j = 0;

		while(args[j] != NULL){

			if(strcmp(args[j], "<") == 0){
				infile = args[j + 1];
				args[j] = NULL;
				break;
			}

			if(strcmp(args[j], ">") == 0){
				outfile = args[j + 1];
				args[j] = NULL;
				break;
			}

			j++;
		}

		int pid = fork();

		if(pid == 0){

			if(infile != NULL){
				close(0);
				int fd = open(infile, O_RDONLY);
				if(fd < 0){
					perror("open");
					exit(1);
				}
			}

			if(outfile != NULL){
				close(1);
				int fd = open(outfile,O_WRONLY | O_CREAT | O_TRUNC,0644);
				if(fd < 0){
					perror("open");
					exit(1);
				}
			}

			myexec(args[0], args);
		}
		else{
			wait(NULL);
		}
	}

	return 0;
}