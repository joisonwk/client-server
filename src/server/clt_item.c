/*src/server/clt_item.c*/
#define __CLT_ITEM_C__
#include <server/server_core.h>
#include <clt_item.h>

void clt_add(struct server_data* pdata, CLT_T* pclt){
	if(pdata == NULL || clt == NULL){
		return;	
	}	
	
	CLT_T* pclt_tmp = pdata->sd_clts;
	while(NULL != pclt_tmp){
		pclt_tmp = pclt_tmp->next;
	}
	pclt_tmp = pclt;	
}

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

/*client receive data*/
extern void* tcp_clt_recv_thread(void* pdata){
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
