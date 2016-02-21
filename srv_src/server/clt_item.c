/*src/client/clt_item.c*/
#define __CLT_ITEM_C__
#include <util/c_list.h>
#include <util/c_kernel.h>
#include <server/server_core.h>
#include <clt_item.h>

/*release the timeout client connect*/
void clt_flush(void){
	CLT_T* pclt = NULL;
	struct list_head* clt_head, *list_pos;
	struct server_data* psd;

	psd = get_server();
	if(psd == NULL){
		return;	
	}
	clt_head = &psd->sd_clt_head;
	post_server();
	
	gettimeofday(&cur_tm, NULL);
	/*timeout client remove for the client list*/
	list_for_each(list_pos,clt_head){
		pclt = list_entry(head,struct clt_item, ci_list);
		if(clt_is_timeout(pclt)){	
			struct list_head* next = pclt->ci_list->next;	//save the next node
			get_server();
			list_del(pclt->ci_list);
			post_server();

			clt_release(pclt);
			free(pclt);
			if(next==head)
				break;
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
	
	list_add(&pclt->ci_list,sd->sd_clt_head);
	post_server();
}

#undef __CLT_ITEM_C__
