#define __TCPSERVER_C__	//file start
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>

/*from self include direcotry*/
#include <cs_common.h>
#include <process/process.h>
#include <server/tcpserver.h>

struct tcp_data* ptcp_server;

int tcp_server_create(void* pdata)
{
	int retval = -1;
	int sockfd;
	struct sockaddr_in server_addr;
	int tcp_domain;
	unsigned short tcp_port;
	struct ts_data* ptcp_data = pdata;

	if(pdata == NULL){
		printf("tcp server is empty\n");
		return -1;
	}
    
    	/*socket argument check*/
	tcp_domain = ptcp_data->td_dev.domain;
	if((tcp_domain!=AF_INET) && (tcp_domain!=AF_INET6)){
		tcp_domain = AF_INET;
	}

	tcp_port = ptcp_data->td_dev.port;
	/*uncomplete: tcp_port testing*/

	//AF_INET: Internet IPV4 Protocol
	//SOCK_STREAM: Sequenced, reliable, connection-based byte streams
	//0: IPPROTO_IP = 0, Dummy protocol for TCP
	if((sockfd = socket(tcp_domain,SOCK_STREAM,0)) == -1)
	{
		printf("create socket error");
		perror("socket");
		return -1;
	}
    
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//host byte order to net
	server_addr.sin_port = htons(TCP_SERVER_PORT);
	//INADDR_ANY: Address to accept any incoming messages
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sockfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr))==-1)
	{
		perror("bind socket error");
		return -1;
	}
    
	//N connection requests will be queued before further requests are refused.
	if(listen(sockfd,ptcp_data->td_dev.max_conn)==-1)
	{
		perror("listen");
		return -1;
	}

	ptcp_data->td_sfd = sockfd;

	return 0;
}

void* tcp_server_conn(void* pdata){
	struct ts_data* ptcp_data= NULL;
	int server_fd, clt_fd;
	struct sockaddr_in cli_addr;
	int sin_size = sizeof(struct sockaddr_in);
	ptcp_data = pdata;

	if(NULL == pdata){
		pthread_exit(NULL);
	}

	server_fd = ptcp_data->td_sfd;


	while(1){
		clt_fd = accept(server_fd, &cli_addr, &sin_size);
		if(clt_fd == -1){
			perror("accept failed");
			pthread_exit(NULL);
		}

		CLT_INFO_T* clt_item = malloc(sizeof(CLT_INFO_T));
		if(clt_item == NULL){
			perror("allocate client structure memory failed");
			pthread_exit(NULL);
		}
		bzero(clt_item, sizeof(CLT_INFO_T));

		clt_item->ci_cfd = clt_fd;
		sem_init(&clt_item->ci_sem,0,0);
		gettimeofday(&clt_item->ci_conn_time, NULL);

		tcp_server_clt_add(ptcp_data, clt_item);
	}
}

int tcp_server_clt_add(struct ts_data* pdata, CLT_INFO_T* clt_item){
	CLT_INFO_T* clt_temp = NULL;
	if(clt_item == NULL){
		printf("clt_item is empty\n");
		return -1;
	}

	/*connect stack full, fresh the tcp server*/
	if(pdata->td_cur_clt_num >= pdata->td_max_conns){
		printf("connectons statck full\n");
		if(tcp_server_connfresh(pdata)){
			printf("fresh tcp server connecton failed\n");
			return -1;	
		}
	}

	clt_temp = pdata->td_clts_head;
	while(clt_temp!=NULL){
		clt_temp = clt_temp->next;
	}
	clt_temp = clt_item;
	pdata->td_cur_clt_num++;
}

/*disconnect timeout connection, or diconnect the oldest connection when the tcp server stack is full*/
int tcp_server_connfresh(void* pdata){
	struct ts_data* ptcp_data = pdata;	
	struct timeval cur_time; 
	time_t secs;

	if(gettimeofday(&cur_time, NULL)!=0){
		perror("gettimeofday failed");
		return -1;
	}

	secs = cur_time->tv_sec;

	if(pdata == NULL){
		printf("tcp server data is NULL\n");
		return -1;
	}

	
}

int tcp_server_clt_del(CLT_INFO_T* clt_item){
	clt_info_release(clt_item);	
}

/*initiate the client info*/
int clt_info_init(CLT_INFO_T* clt_item){
		
}

/*release the client info */
void clt_info_release(CLT_INFO_T* clt_item){
	if(clt_item == NULL){
		return;
	}
	close(clt_item->ci_cfd);
}

void* tcp_server_recv(void* pdata){
	if(pdata == NULL){
		pthread_exit(NULL);
	}

	struct ts_data* ptcp_data = (struct ts_data)pdata;
	FD_ZERO(&ptcp_data->td_recv_fds);

	while(1){
		select(fd);
	}

	struct ts_data* tcp_data = pdata;

}

void* tcp_server_process(void* pdata){
	struct ts_data ptcp_data = pdata;
	if(pdata == NULL){
		pthread_exit(NULL);
	}
	while(1){
		CLT_INFO_T* cli_list = ptcp_data->td_clts_head;
		CLT_INFO_T* cli_temp = cli_list;
		while(cli_temp){
			if(cli_temp->){
				
			}
			cli_temp  =cli_temp->next;
		}
		usleep(100000);	//each 100ms check the cli_list;
	}
}

/*configure the tcp server with platform data*/
int tcp_server_init(void* pdev){
	int ret = -1;
	/*1.0 allocate resource*/
	ptcp_server = malloc(sizeof(struct ts_data));

	if(ptcp_server == NULL){
		perror("allocate tcp_server failed");
		return -1;
	}
	/*1.1 initiate tcp server resource*/
	bzero(ptcp_server, sizeof(struct ts_data));
	memcpy(ptcp_server->td_dev, pdev, sizeof(struct ts_dev));
	ptcp_server->td_max_conns = ptcp_server->td_dev.max_conn;

	/*1.2 create tcp server */
	ret = tcp_server_create(ptcp_server);
	if(ret < 0){
		printf("tcp server create failed");	
		goto err_server_create;
	}
	/*2.0 initiate fdsets*/
	FD_ZERO(&ptcp_server->td_recv_fds);
	FD_ZERO(&ptcp_server->td_snd_fds);

	/*3.0 create threads for connection, receive and data process*/
	/*3.1 create tcp server accept thread*/
	ret = pthread_create(&tcp_server->td_conn_pid,NULL, tcp_server_conn, ptcp_server);
	if(ret != 0){
		printf("connection thread create failed\n");
		goto err_conn_thread;
	}

	/*3.2 create data recieve thread*/
	ret = pthread_create(&tcp_server->td_recv_pid, NULL, tcp_server_recv, ptcp_server);
	if(ret != 0){
		printf("receive thread create failed\n");
		goto err_recv_thread;
	}

	/*3.3 create data process thread*/
	ret = pthread_create(&tcp_server->td_process_pid, NULL, tcp_server_process, ptcp_server);
	if(ret != 0){
		printf("process thread create failed\n");
		goto err_process_thread;
	}

	return 0;

err_process_thread:
/*kill receive thread*/
err_recv_thread:
/*kill connection thread*/
err_conn_thread:
/*close tcp server socket*/
	close(ptcp_server->td_sfd);	
err_server_create:
	free(ptcp_server);
	return ret;
}


#undef __TCPSERVER_C__//file end
