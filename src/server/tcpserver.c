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
#include <errno.h>

/*from self include direcotry*/
#include <cs_common.h>
#include <process/process.h>
#include <server/tcpserver.h>

struct tcp_data* ptcp_server;

/*create tcp server*/
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

/*receive client connection and insert to client stack*/
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

		CLT_INFO_T* pclt_item = malloc(sizeof(CLT_INFO_T));
		if(pclt_item == NULL){
			tcp_server_connfresh(ptcp_data);
			pclt_item = malloc(sizeof(CLT_INFO_T));
			if(pclt_item == NULL){
				perror("allocate client structure memory failed");
				pthread_exit(NULL);
			}
		}

		bzero(pclt_item, sizeof(CLT_INFO_T));
		clt_init(pclt_item);
		pclt_item->ci_cfd = clt_fd;

		if(0 != tcp_server_clt_add(ptcp_data, clt_item)){
			printf("add client failed");
			clt_release(pclt_item);
			if(pclt_item){
				free(pclt_item);
				pclt_item = NULL;
			}
		}
	}
}

/*initiate the client info struction*/
void clt_init(CLT_INFO_T* pclt){
	if(NULL == pclt){
		return;
	}
	/*initiate the client semophore*/
	sem_init(pclt->ci_sem,0,0);
	/*recording the initiate time*/
	gettimeofday(&pclt->ci_conn_time, NULL);
}

/*release the client resource*/
void clt_release(CLT_INFO_T* pclt){
	if(NULL == pclt){
		return;
	}	

	sem_close(pclt->ci_sem);
	/*close the client socket*/
	if(pclt->ci_cfd){
		close(pclt->ci_cfd);	
	}
}
void clt_lock_init(CLT_INFO_T* clt){

}
int clt_lock_get(CLT_INFO_T* clt){

}

void clt_lock_release(CLT_INFO_T* clt){

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

	/*insert clt_item to the stack*/
	clt_temp = pdata->td_clts_head;
	while(clt_temp!=NULL){
		clt_temp = clt_temp->next;
	}
	clt_temp = clt_item;

	/*set fd to recv sets and snd sets*/
	FD_SET(clt_item->ci_cfd, &pdata->td_recv_fds);
	FD_SET(clt_item->ci_cfd, &pdata->td_snd_fds);

	/*current client number */
	pdata->td_cur_clt_num++;
}

/*disconnect timeout connection, or diconnect the oldest connection when the tcp server stack is full*/
int tcp_server_connfresh(struct ts_data* pdata){
	struct ts_data* ptcp_data = pdata;	
	struct timeval cur_time; 
	time_t cur_secs;

	if(pdata == NULL){
		printf("tcp server data is NULL\n");
		return -1;
	}

	/*when current client number not less than the max connections, or no space*/
	if((ptcp_data->td_max_conns <= ptcp_data->td_cur_clt_num) ||
		(errno==ENOMEM)){
		int pre_conns = ptcp_data->td_cur_clt_num;
		CLT_INFO_T* clt_temp,clt_it;

		if(gettimeofday(&cur_time, NULL)!=0){
			perror("gettimeofday failed");
			return -1;
		}
		cur_secs = cur_time->tv_sec;

		clt_it= ptcp_data->td_clts_head;
		while(clt_it){
			clt_temp = clt_it;
			clt_it= clt_it->next;

			time_t persist_sec = 0;
			persist_sec = cur_secs - clt_temp->ci_conn_time.tv_sec;	//persistent time

			/*release and delete client */
			if(persist_sec > ptcp_data->td_clt_threshold){
				FD_CLR(clt_temp->ci_cfd, &ptcp_data->td_recv_fds);
				FD_CLR(clt_temp->ci_cfd, &ptcp_data->td_snd_fds);
				clt_release(clt_temp);
				free(clt_temp);
			}
		}
	}

}

/*tcp server receive data from client socket*/
void* tcp_server_recv(void* pdata){
	if(pdata == NULL){
		pthread_exit(NULL);
	}

	struct ts_data* ptcp_data = (struct ts_data)pdata;
	FD_ZERO(&ptcp_data->td_recv_fds);

	while(1){
		int nfds = ptcp_data->td_max_conns;
		fd_set rfds, sfds;
		rfds = ptcp_data->td_recv_fds;
		sfds = ptcp_data->td_snd_fds;
		CLT_INFO_T* cli_list = ptcp_data->td_clts_head;

		int sct_ret = select(nfds, &rfds, &sfds, NULL, 0);
		if(sct_ret <= 0){
			continue;
		}
		/*iterate test client socket*/
		for(;cli_list;cli_list=cli_list->next){
			/*receive data from client */
			if(FD_ISSET(cli_list->ci_cfd, &rfds) && clt_lock_get(cli_list)){
				cli_list->ci_recv_len = recv(cli_list->cfd, cli_list->ci_recvbuf, MAX_BUFF_LEN,0);
				clt_lock_release(cli_list);
			}

			/*send data to client*/
			if((cli_list->ci_sndlen>0) && FD_ISSET(cli_list->ci_cfd, &sfds) && clt_lock_get(cli_list)){
				char* sndbuf = cli_list->ci_sndbuf;
				int slen = cli_list->ci_sndlen;
				int sfd = cli_list->ci_cfd;
				while(slen>0){
					slen -= send(sfd, sndbuf, slen, 0);
				}
				cli_list->ci_sndlen = 0;
				clt_lock_release(cli_list);
			}
		}
	}
}

void* tcp_server_process(void* pdata){
	struct ts_data ptcp_data = pdata;
	if(pdata == NULL){
		pthread_exit(NULL);
	}

	/*process the client request*/
	while(1){
		if(ptcp_data->td_max_conns == 0){
			usleep(100000); 
			continue;
		}

		CLT_INFO_T* cli_temp = ptcp_data->td_clts_head;
		while(cli_temp){
			int rlen, slen;
			char* rbuf, sbuf;
			rbuf = cli_temp->ci_recvbuf;
			rlen = cli_temp->ci_recv_len;
			sbuf = cli_temp->ci_sndbuf;
			slen = cli_temp->ci_sndlen;

			if(clt_lock_get(cli_temp) && (rlen>0)){
				/*parse the rbuf*/
				if(rlen>0){
					slen += id_process(rbuf,rlen,sbuf+slen);
				}
				clt_lock_release(cli_temp);
			}

			cli_temp  =cli_temp->next;
		}
	}
}

/*ts_lock_init*/
void tcp_server_lock_init(struct ts_data* pdata){
}

/*ts_data get lock*/
int tcp_server_lock_get(struct ts_data* pdata){
}

/*ts_data release lock*/
void tcp_server_lock_release(struct ts_data* pdata){
}

/*configure the tcp server with platform data*/
int tcp_server_init(void* pdev){
	int ret = -1;
	struct ts_dev* pts_dev = (struct ts_dev*)pdev;
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
	
	tcp_server_lock_init(ptcp_server);
	/*set the client time threshold*/
	if((pts_dev->tm_threshold<=0) || (pts_dev->tm_threshold>MAX_THRESHOLD)){
		ptcp_server->td_clt_threshold = DEFAULT_THRESHOLD;
	}

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