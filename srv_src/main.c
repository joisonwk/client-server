/*src/main.c*/
#include <stdio.h>
#include <server/server_core.h>
#include <server/clt_item.h>
#include <conn/conn_tcp.h>
#include <process/process_core.h>

int main(int argc, char* argv[]){
	int retval = 0;
	int connflag = 0;
	pthread_t *ptcp_pid, *pprcs_pid;
	/*initial the server, if failed return*/
	if(server_init()){
		printf("server create failed\n");	
		return -1;
	}else{
		printf("server init success...\n");
	}

	struct server_data* psd = get_server();
	ptcp_pid = &psd->sd_tcp_pid;
	pprcs_pid = &psd->sd_prcs_pid;
	post_server();
	/*get the server handle and locking it*/
	/*#create tcp server thread*/
	retval = pthread_create(ptcp_pid,NULL,tcp_thread,NULL);
	if(retval==0){
		printf("tcp server thread create success...\n");
		connflag = 1;
	}else{
		printf("tcp server thread create failed returned %d\n", retval);
	}

	{
		/*#uncomplete:create udp server*/
		//udp_init(sd);
	}
	if(connflag==1){
		retval=pthread_create(pprcs_pid, NULL,process_thread, NULL);
		if(retval==0){
			printf("process thread create success...\n");
		}else{
			printf("process thread create failed returned %d\n", retval);	
			return -1;
		}
	}


	/*release the server handle*/
	pthread_join(*ptcp_pid, NULL);
	printf("tcp thread returned\n");
	pthread_join(*pprcs_pid, NULL);

	/*release all client items*/
	clt_exit();
}

