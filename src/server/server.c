/*src/server/server.c*/
#define __SERVER_C__
#include <stdio.h>
#include <pthread.h>
#include <server/server_core.h>
#include <server/conn_tcp.h>
#include <server/clt_item.h>
#include <server/process_server.h>

static struct server_data* sd = NULL;
/*release timeout client and process data*/
void flush_sys(void){
	clt_flush();	
	process_flush();
}

int server_init(void* conf){
	if(sd == NULL){
		sd = (struct server_data*)malloc(struct server_data);	
	}
		if(NULL == sd)
		{
			perror("server data allocate failed");
			return -1;
		}
	sd->sd_udp_port = UDP_PORT;
	sd->sd_tcp_port = TCP_PORT;
	sem_init(&sd->sd_sem, 0, 1);
	INIT_LIST_HEAD(&sd->sd_clt_head);	
	INIT_LIST_HEAD(&sd->sd_pd_head);
}

struct server_data* get_server(void){
	sem_wait(&sd->sd_sem);	
	return sd;
}

void post_server(void){
	sem_post(&sd->sd_sem);
}

#undef __SERVER_C__
