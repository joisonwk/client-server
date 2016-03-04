#define __CONN_TCP_C__	//file start
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*from self include direcotry*/
#include <util/c_kernel.h>
#include <util/c_list.h>
#include <server/server_core.h>
#include <server/clt_item.h>
#include <common.h>
#include <conn/conn_tcp.h>

static int sfd = 0;	//tcp server socket

/*create tcp server*/
int tcp_init(void)
{
	struct sockaddr_in server_addr;
	int tcp_domain;
	struct tcp_data* ptd = NULL;
	struct server_data* psd = get_server();
	if(psd==NULL)
		return -1;
	ptd = &psd->sd_tcpconf;
	post_server();

	if(ptd==NULL){
		printf("tcp config is empty\n");
		return -1;
	}

    	/*socket argument check*/
	tcp_domain = ptd->tcp_domain;
	if((tcp_domain!=AF_INET) && (tcp_domain!=AF_INET6)){
		tcp_domain = AF_INET;
	}

	/*uncomplete: tcp_port testing*/
	/*1.1 create socket*/
	if((sfd = socket(tcp_domain,SOCK_STREAM,0)) == -1)
	{
		printf("create socket error");
		perror("socket");
		return -1;
	}
    
    	/*1.2 initial the socket address*/
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = tcp_domain;
	//host byte order to net
	server_addr.sin_port = htons(ptd->tcp_port);
	//INADDR_ANY: Address to accept any incoming messages
	server_addr.sin_addr.s_addr = INADDR_ANY;

	/*1.3 bind socket address with socket*/
	if(bind(sfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr))==-1)
	{
		perror("bind socket error");
		close(sfd);
		return -1;
	}
    
	//N connection requests will be queued before further requests are refused.
	if(ptd->tcp_max_lstn <= 0)
		ptd->tcp_max_lstn = DEFAULT_LSTNUM;
	if(listen(sfd,ptd->tcp_max_lstn)==-1)
	{
		perror("listen");
		close(sfd);
		return -1;
	}
	ptd->tcp_sfd = sfd;

	//printf("tcp init success, server socket fd =%d\n", sfd);
	return 0;
}

/*receive client connection and insert to client stack*/
void* tcp_thread(void* pdata){
	struct server_data *psd = NULL;
	struct tcp_data* ptd;
	int clt_fd;
	struct sockaddr_in cli_addr;
	int sin_size = sizeof(struct sockaddr_in);
	
	psd = get_server();
	ptd = &psd->sd_tcpconf;
	post_server();

	if(tcp_init()) {
		perror("init tcp server failed\n");
		pthread_exit(NULL);	
	}

	if(pthread_create(&ptd->tcp_recv_pid,NULL,tcp_clt_data_recv,NULL)){
		printf("create tcp client data recieve thread failed\n");
		pthread_exit(NULL);
	}else{
		printf("create tcp client data recieve thread success, pid=%d\n", ptd->tcp_recv_pid);
	}
	if(pthread_create(&ptd->tcp_send_pid,NULL,tcp_clt_data_send,NULL)){
		printf("create tcp client data send thread failed\n");
		pthread_exit(NULL);
	}else{
		printf("create tcp client data send thread success, pid=%d\n", ptd->tcp_send_pid);
	}

	while(1){
		clt_fd = accept(sfd, (struct sockaddr*)&cli_addr, &sin_size);
		if(clt_fd == -1){
			perror("accept failed");
			pthread_exit(NULL);
		}else{
			printf("recieved client socket fd=%d\n", clt_fd);
		}

		CLT_T* pclt_item = NULL;
		pclt_item = clt_malloc();
		if(pclt_item == NULL){
			char tbuf[128] = "server have no memory";
			printf("allocate client memory failed\n");
			send(clt_fd, tbuf, strlen(tbuf), 0);
			close(clt_fd);
			continue;
		}

		clt_init(pclt_item);
		pclt_item->ci_cfd = clt_fd;
		pclt_item->ci_type = ECT_TCP;

		/*set tcp client socket as noblocking*/
		int flag = fcntl(clt_fd, F_GETFD, NULL);
		flag |= O_NONBLOCK;
		fcntl(clt_fd, F_SETFD,&flag);

		/*insert client to list failed close the client socket and release it*/
		if(clt_add(pclt_item)){
			clt_release(pclt_item);
			free(pclt_item);
			close(clt_fd);
			continue;
		}
		//printf("add client %d to list success\n", clt_fd);
		
		pclt_item->ci_cfd = clt_fd;
	}

	pthread_join(ptd->tcp_recv_pid,NULL);
	pthread_join(ptd->tcp_send_pid,NULL);
	pthread_exit(NULL);
}

/*client memory allocat*/

/*tcp client receive data from client socket*/
void* tcp_clt_data_recv(void* pdata){
	CLT_T* pclt = NULL;
	struct server_data* psd = get_server();
	struct list_head* phead = &psd->sd_clt_head;
	post_server();
	while(1){
		list_for_each_entry(pclt,phead, ci_list){
			/*recieve data*/
			if(pclt->ci_type==ECT_TCP && !sem_trywait(&pclt->ci_sem)){
				int cfd = pclt->ci_cfd;
				//printf("[%d] sem is locking by recv thread....\n", cfd);
				char* rbuf = pclt->ci_rbuf;
				int bufsize = sizeof(pclt->ci_rbuf);
				int rlen = pclt->ci_rlen;
				int rcvret = 0;
				/*does the recv func will block?*/
				rcvret = recv(cfd,rbuf+rlen,bufsize-rlen,MSG_DONTWAIT);
				if(rcvret > 0){
					//printf("[%d] RECV [%s]\n", cfd, rbuf);
					pclt->ci_rlen += rcvret;
					clt_fresh(pclt);
				}
				sem_post(&pclt->ci_sem);
				//printf("[%d] sem is unlocked\n", cfd);
			}
		}
	}
}
/*tcp client data send */
void* tcp_clt_data_send(void* pdata){
	CLT_T* pclt = NULL;
	struct server_data* psd = get_server();
	struct list_head* phead = &psd->sd_clt_head;
	post_server();
	while(1){
		list_for_each_entry(pclt,phead,ci_list){
			if(pclt->ci_type==ECT_TCP && !sem_trywait(&pclt->ci_sem)){
				int cfd = pclt->ci_cfd;
				//printf("[%d] is locking by send thread\n",cfd);
				int wlen = pclt->ci_wlen;
				char* wbuf = pclt->ci_wbuf;
				int offset,sndret;
				offset = sndret = 0;
				int rept = 3;	//repeat thread times if failed
				while(wlen>0 && rept >0){
					sndret = send(cfd, wbuf+offset,wlen,MSG_DONTWAIT);		
					sndret>0? (wlen-=sndret,offset+=sndret):(rept--);
				}
				/*move not sended data to buf header*/
				pclt->ci_wlen = wlen;
				if(wlen>0){
					memcpy(wbuf,wbuf+offset,wlen);
				}

				sem_post(&pclt->ci_sem);
				//printf("[%d] is unlocked by send thread\n", cfd);
			}
		}
	}
	pthread_exit(NULL);
}

#undef __CONN_TCP_C__//file end
