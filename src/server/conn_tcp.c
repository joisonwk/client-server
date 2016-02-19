#define __CONN_TCP_C__	//file start
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <errno.h>

/*from self include direcotry*/
#include <util/c_kernel.h>
#include <server/server_core.h>
#include <common.h>
#include <server/conn_tcp.h>

static int sfd;	//tcp server socket

/*create tcp server*/
int tcp_init(struct tcp_data* pdata)
{
	int sfd;
	struct sockaddr_in server_addr;
	int tcp_domain;
	struct server_data* psd = NULL;

	if(pdata == NULL){
		printf("tcp server is empty\n");
		return -1;
	}

	psd = container_of(pdata,struct server_data,sd_tcpconf);
    
    	/*socket argument check*/
	tcp_domain = pdata->td_dev.domain;
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
	server_addr.sin_family = AF_INET;
	//host byte order to net
	server_addr.sin_port = htons(pdata->tcp_port);
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
	if(pdata->tcp_max_lstn <= 0){
		pdata->tcp_max_lstn = DEFAULT_LSTNUM;
	}
	if(listen(sfd,pdata->tcp_max_lstn)==-1)
	{
		perror("listen");
		close(sfd);
		return -1;
	}
	pdata->tcp_sfd = sfd;
	psd->sd_conn_flag = true;

	return 0;
}

/*receive client connection and insert to client stack*/
void tcp_thread(void* pdata){
	struct server_data *psd = pdata;
	struct tcp_data* ptd;
	int clt_fd, srv_fd;
	struct sockaddr_in cli_addr;
	int sin_size = sizeof(struct sockaddr_in);

	if(NULL == pdata) {
		pthread_exit(NULL);
	}

	ptd = &psd->sd_tcpconf;
	if(tcp_init(&psd->sd_tcpconf)) {
		perror("init tcp server failed\n");
		pthread_exit(NULL);	
	}

	pthread_create(ptd->tcp_recv_pid,NULL,tcp_clt_data_recv,psd);
	pthread_create(ptd->tcp_send_pid,NULL,tcp_clt_data_send,psd);

	while(1){
		clt_fd = accept(srv_fd, (struct sockaddr_in*)&cli_addr, &sin_size);
		if(clt_fd == -1){
			perror("accept failed");
			pthread_exit(NULL);
		}

		CLT_T* pclt_item = NULL;
		pclt_item = clt_malloc();
		if(pclt_item == NULL){
			char tbuf[128] = "server have no memory";
			send(clt_fd, tbuf, strlen(tbuf));
			close(clt_fd);
			continue;
		}

		clt_init(pclt_item);

		pclt_item->ci_cfd = clt_fd;
		pclt_item->ci_type = ECT_TCP;
		/*set tcp client socket as noblocking*/
		int flag = fcntl(clt_fd, F_FDGET, NULL);
		flag |= O_NONBLOCK;
		fcntl(clt_fd, F_FDSET,&flag);

		clt_add(pclt_item);
		
		pclt_item->ci_cfd = clt_fd;
	}

	pthread_join(ptd->tcp_recv_pid,NULL);
	pthread_join(ptd->tcp_send_pid,NULL);
	pthread_exit(NULL);
}

/*client memory allocat*/

/*tcp client receive data from client socket*/
void* tcp_clt_data_recv(void* pdata){
	struct server_data psd = (struct server_data*)pdata;
	if(psd == NULL)
		pthread_exit(NULL);
	CLT_T* pclt = NULL;
	struct list_head* phead = psd->sd_clt_head;
	while(1){
		list_for_each_entry(pclt,phead, ci_head){
			/*recieve data*/
			if(pclt->ci_type==ECT_TCP && !sem_trywait(&pclt->ci_sem)){
				int cfd = pclt->ci_cfd;
				char* rbuf = pclt->ci_rbuf;
				int rlen = pclt->ci_rlen
				int rcvret = 0;
				/*does the recv func will block?*/
				rcvret = recv(ci_cfd,rbuf+rlen,bufsize-rlen);
				if(rcvret > 0){
					pclt->ci_rlen += rcvret;
					clt_fresh(pclt);
				}
				sem_post(&pclt->ci_sem);
			}
		}
	}
}
/*tcp client data send */
void* tcp_clt_data_send(void* pdata){
	struct server_data* psd = (struct server_data*)pdata	
	CLT_T* pclt = NULL;

	if(psd==NULL) pthread_exit(NULL);
	
	struct list_head* phead = psd->sd_clt_head;
	while(1){
		list_for_each_entry(){
			if(pclt->ci_type==ECT_TCP && !sem_trywait(&pclt->ci_sem)){
				int cfd = pclt->ci_cfd;
				int wlen = pclt->ci_wlen;
				char* wbuf = pclt->ci_wbuf;
				int offset = sndret = 0;
				int rept = 3;
				while(wlen>0 && rept >0){
					sndret = send(cfd, wbuf+offset,wlen);		
					sndret>0? (wlen-=sndret,offset+=sndret):(rept--);
				}
				if(wlen>0){
					pclt->ci_wlen = wlen;
					memcpy(wbuf,wbuf+offset,wlen);
				}

				sem_post(&pclt->ci_sem);
			}
		}

	}
}

#undef __CONN_TCP_C__//file end
