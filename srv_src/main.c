/*src/main.c*/
#include <stdio.h>
#include <server/server_core.h>
#include <server/conn_tcp.h>
#include <server/process_server.h>
#include <server/clt_item.h>
int main(int argc, char* argv[]){
	/*initial the server, if failed return*/
	if(!server_init()){
		printf("server create failed\n");	
		return -1;
	}

	struct server_data* psd = NULL;
	/*get the server handle and locking it*/
	/*#create tcp server thread*/
	pthread(&psd->sd_tcp_pid,NULL,tcp_thread,NULL);

	{
		/*#uncomplete:create udp server*/
		//udp_init(sd);
	}

	/*create data process thread*/
	if(psd->sd_conn_flag){
		pthread_create(&psd->sd_prcs_pid, NULL,process_thread, NULL);
	}

	/*release the server handle*/
	pthread_join(sd->sd_prcs_pid, NULL);
	pthread_join(sd->sd_tcp_pid,NULL);
	/*#create data deal thread*/
}

