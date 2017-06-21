#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFSIZE 4096

int main(int argc, char **argv){
	if(argc != 2){
		printf("usage %s <service name>\n",argv[0]);
		exit(1);
	}
	
	int name_len = strlen(argv[1]) + 6 + 1;
	char name[name_len];
	strcpy(name,"../ro/");
	strcat(name,argv[1]);
	//printf("%s\n",name);
	char buff[BUFFSIZE +1 ];	
	int fd[2], quit = 1;
	if(pipe(fd) == -1){
		perror("pipe");
		exit(1);
	} 	
	int pid;
	if( (pid = fork()) == 0 ){	//sono il processo figlio
		close(0);
		dup(fd[0]);
		if (execl(name, NULL) == -1 ){
			printf("Non ha funzionato la exec\n");
		}
	}else{	//sono il processo padre
		int rd;
		while(quit){
			if(waitpid(pid,NULL,WNOHANG)){
				printf("ha fatto\n");
				return 0;
			}
			rd = read(0,buff,BUFFSIZE-2);
			buff[rd] = '\n';
			buff[rd+1] = '\0';
			//printf("Padre: %s",buff);
			if( strstr(buff,"@") != NULL ){
				printf("AH! Volevi\n");
				return 0;
			}
			write(fd[1],buff,rd);
			usleep(500);
		}
	}

	return 0;
}
