/*src/server/clt_item.c*/
#define __CLT_ITEM_C__
#include <server/server_core.h>
#include <clt_item.h>

/*release the timeout client connect*/
void clt_flush(struct server* pserver){
	CLT_T* pclt = NULL;
	struct timeval cur_tm;
	if(pserver == NULL){
		return;	
	}
	
	gettimeofday(&cur_tm, NULL);
	CLT_T* pclt = pserver->sd_clts;
	while(pclt){
		if(pclt->ci_conn_time.tv_sec+CLT_TIMEOUT<cur_tm.tv_sec){
			CLT_T* ptmp = pclt;
			clt_release(pclt);
			free(pclt);
		}
	}
}

/*client memory allocate*/
CLT_T* clt_malloc(void){
	CLT_T* item = (CLT_T*)malloc(sizeof(CLT_T));
	/*flush client to release memory space*/
	if(item == NULL){
		flush_sys();	//release space
		item = (CLT_T*)malloc(sizeof(CLT_T));
	}
	if(item != NULL)
		bzero(item, sizeof(CLT_T));
	return item;
}

/*update client timer*/
void clt_fresh(CLT_T* pclt){
	if(pclt==NULL || pclt->ci_timeval==0){
		return;
	}	
	gettimeofday(&pclt->ci_last_time, NULL);
}

/*test client whethe overtime*/
static bool clt_is_timeout(CLT_T* pclt){
	struct timeval curtm;
	if(pclt==NULL || pclt->ci_timeval==0){
		return false;
	}else if(pclt->ci_timeval>MAX_TIMEVAL){
		pclt->ci_timeval = MAX_TIMEVAL;		
	}
	gettimeofday(&curtm, NULL);
	return (pclt->ci_timeval.tv_sec+pclt->ci_last_time.tv_sec < curtm.tv_sec? true:false);
}

/*client initiate*/
extern void clt_init(CLT_T* pclt){
	sem_init(&pclt->ci_sem, 0, 1);
	INIT_LIST_HEAD(&pclt->ci_list);
}

extern void clt_add(CLT_T* pclt){
	struct server_data* sd = get_server();
	if(sd==NULL){
		return;
	}
	list_add(pclt,sd->sd_clt_head);
}

void clt_send(CLT_T* pclt){
	switch(pclt->ci_type){
	case CONN_TCP:
	case CONN_UDP:
		tcp_clt_data_send(pclt);
		pclt->ci_wlen -= send(pclt->ci_cfd,pclt->ci_wbuf, pclt->ci_wlen);	
		break;
	default:
		/*uncomplete:*/	
	}
}

void* clt_recv_data(void* pdata);

/*client receive data*/
void* clt_recv_thread(void* pdata){
	struct server_data* pserver = pdata;
	int sfd = pserver->sd_fd;
	struct sockaddr_in caddr;
	size_t sin_size = 0;
	while(1){
		int cfd = (sfd, &caddr, &sin_size, 0);
		if(cfd>0){
			CLT_T* pclt = (CLT_T*)malloc(sizeof(CLT_T));	
			if(pclt==NULL){
				flush_sys();
			}
		}
		
	}
}

#undef __CLT_ITEM_C__
