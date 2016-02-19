/*src/main.c*/
#include <stdio.h>
#include <server/server_core.h>
#include <server/conn_tcp.h>
#include <server/process_server.h>
#include <server/clt_item.h>
int main(int argc, char* argv[]){
	if(!server_init()){
		printf("server create failed\n");	
	}

	struct server_data* psd = NULL;
	psd = get_server();
	/*#create tcp server thread*/
	pthread(&psd->sd_tcp_pid,NULL,tcp_thread,psd);

	{
		/*#uncomplete:create udp server*/
		//udp_init(sd);
	}

	/*create data process thread*/
	if(psd->sd_conn_flag){
		pthread_create(&psd->sd_prcs_pid, NULL,process_thread, psd);
	}

	post_server();	//server init completed
	pthread_join(sd->sd_prcs_pid, NULL);
	pthread_join(sd->sd_tcp_pid,NULL);
	/*#create data deal thread*/
}

