/*src/server/server.c*/
#define __SERVER_C__
#include <stdio.h>
#include <pthread.h>
#include <server_core.h>
#include <conn_common.h>

struct server_data sd;
/*release timeout client and process data*/
void flush_sys(void){
	clt_flush(pserver);	
	flush_process();
}

int main(int argc, char* argv[]){
	bzero(&sd);

	sd->sd_udp_port = UDP_PORT;
	sd->sd_tcp_port = TCP_PORT;
	/*#create tcp server*/
	if(tcp_init(sd)){
		if(pthread_create(&sd->sd_recvpid,NULL,tcp_clt_recv_thread(&sd)));
	}
	/*#uncomplete:create udp server*/
	//udp_init(sd);

	/*#create data deal thread*/
	
}

#undef __SERVER_C__
