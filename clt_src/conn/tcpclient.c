#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <conn_item.h>

struct conn_item* pci = NULL;
int sockfd;

int tcp_init(struct conn_item* pdata)
{
    	struct sockaddr_in* paddr;
	pci = pdata;
	
	if(NULL == pci){
		printf("connect item is empty\n");
		return -1;
	}
	paddr = &pci->ci_saddr;
    
	/*create psockfd*/
	if(sockfd==0){
		sockfd = socket(AF_INET, SOCK_STREAM,0);
  		if(sockfd==-1){
			perror("socket");
    			printf("create socket error\n");
			return -1;
		}
	}
	printf("socket fd = %d\n", sockfd);

    	bzero(&(paddr->sin_zero),8);
    
    	if(connect(sockfd,(struct sockaddr *)&paddr,sizeof(struct sockaddr))==-1)
    	{
		perror("connect");
		exit(1);
    	}
	return 0;
}
    
int id_to_addr(const char* id_buf,char* addr_buf, int* paddrlen){
    	if(send(pci->ci_sockfd,id_buf,strlen(id_buf),0)==-1)
    	{
		if(tcp_init(pci)){
			if(-1==send(pci->ci_sockfd,id_buf,strlen(id_buf),0)){
				perror("send");
				return -1;
			}
		}
    	}
    
    	if((*paddrlen= recv(pci->ci_sockfd,addr_buf,*paddrlen,0))==-1)
    	{
        	perror("recv");
		return -2;
    	}

    	addr_buf[*paddrlen] = '\0'; 
    	return 0;
}
