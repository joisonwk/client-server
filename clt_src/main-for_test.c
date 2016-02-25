#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <conn_item.h>
#include <stdlib.h>
#define ID_BUF_LEN 128

void usage(const char* cmdstr){
	printf("please using:\n"
		"%s xx.xx.xx.xx ID", cmdstr);
}

int main(int argc, char* argv[]){
	struct conn_item conn_tcp;
	char get_buf[128];
	int getlen = 0;
	char id_buf[ID_BUF_LEN];
	char addr_buf[516];
	int addrlen = 516;
	/*
	printf("argc = %d\n", argc);
	while(argc--){
		printf("%s ", argv[argc]);
	}
	printf("\n");
	*/
	if(argc!=2){
		usage("client-test");
		exit(1);
	}	

    	conn_tcp.ci_saddr.sin_family = AF_INET;
    	conn_tcp.ci_saddr.sin_port = htons(TCP_PORT);
	if(inet_pton(AF_INET,argv[0],&conn_tcp.ci_saddr.sin_addr)<0){
        	printf("[%s] is not a valid IPaddress\n", argv[0]);
        	exit(2);
	}
	if(tcp_init(&conn_tcp)){
		exit(3);
	}

	while(1){
		bzero(id_buf, sizeof(id_buf));
		/*add header*/
		snprintf(id_buf,ID_BUF_LEN,"IDTOADDR%s",argv[1]);
		getlen = strlen(id_buf);
		if(getlen>0){
			getlen = strlen(id_buf);
			if(!id_to_addr(id_buf,addr_buf,&addrlen)){
				printf("PID[%d]:ID[%s]=ADDR[%s]\n",getpid(),argv[1],addr_buf);
			}else{
				exit(4);
			}
		}
		while(1);	//wait
		usleep(500000);
	}
	
	return 0;
}
