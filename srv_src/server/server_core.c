/*src/server/server_core.c*/
#define __SERVER_core_C__
#include <stdio.h>
#include <pthread.h>
#include <server/server_core.h>
#include <conn/conn_tcp.h>
#include <server/clt_item.h>
#include <process/process_core.h>

static struct server_data* sd = NULL;

/*release system resource*/
void flush_sys(void){
	/*release the timeout client*/
	clt_flush();	
	/*release the timeout process method*/
	process_flush();
}

/*
*function:loading configure from file, conf indicate the file path
* success return 0, failed return -1 and (set the errno, uncomplete)
*/
//int server_init(void* conf){	//uncomplete
int server_init(void){
	if(sd == NULL){
		sd = (struct server_data*)malloc(sizeof(struct server_data));	
		if(sd==NULL){
			perror("initiate server failed\n");
			return -1;
		}else{
			bzero(sd, sizeof(struct server_data));
		}
	}

	/*initiate the udp server struct*/
	sd->sd_udpconf.ud_port = UDP_PORT;
	sd->sd_tcpconf.tcp_port = TCP_PORT;
	sem_init(&sd->sd_sem, 0, 1);	//initiate the semaphere with 1 value
	INIT_LIST_HEAD(&sd->sd_clt_head);	
	//INIT_LIST_HEAD(&sd->sd_pm_head);	uncomplete

	return 0;
}

struct server_data* get_server(void){
	if(sd == NULL)
		server_init();
	sem_wait(&sd->sd_sem);	
	return sd;
}

void post_server(void){
	if(sd==NULL){ 
		server_init();
	}
	sem_post(&sd->sd_sem);
}

#undef __SERVER_CORE_C__
