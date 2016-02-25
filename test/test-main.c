#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CMD_NAME "client-test"

int main(int argc, char* argv[]){
	int fk_num = 0;
	char cmd_buf[128] = {0};
	int i;
	pid_t pid;

	if(argc != 4){
		printf("usage: %s SERVER-IP connNum CITIZEN-ID\n", argv[0]);
		printf("such as: %s 127.0.0.1 16 430524198208020321\n", argv[0]);
	}

	for(i=0; i<atoi(argv[2]); i++){
		pid = fork();	
		if(pid < 0){
			break;
		}else if(pid > 0){
			printf(">>>>>%s create PROCESS[%d] LOADING[%s]\n", argv[0], pid, CMD_NAME);
		}else{
			execl(CMD_NAME, argv[1], argv[3], NULL);
		}
	}
	while(1);
}
