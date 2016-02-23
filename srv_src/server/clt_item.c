/*src/client/clt_item.c*/
#define __CLT_ITEM_C__
#include <stdlib.h>
#include <util/c_list.h>
#include <util/c_kernel.h>
#include <server/server_core.h>
#include <server/clt_item.h>

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
	
	/*timeout client remove for the client list*/
	for(list_pos=clt_head->next;list_pos!=clt_head;){
		pclt = list_entry(list_pos,struct clt_item, ci_list);
		if(clt_is_timeout(pclt)){	
			struct list_head* next = list_pos->next;	//save the next node
			get_server();
			list_del(&pclt->ci_list);
			post_server();

			clt_release(pclt);
			free(pclt);
			if(next==clt_head){
				break;
			}else{
				list_pos = next;
			}
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
	if(pclt==NULL ||
		gettimeofday(&pclt->ci_last_time, NULL)){}
}

/*clt_exit*/
void clt_exit(void){
	CLT_T* pclt = NULL;
	struct list_head* clt_head, *list_pos;
	struct server_data* psd;

	psd = get_server();
	if(psd == NULL){
		return;	
	}
	clt_head = &psd->sd_clt_head;
	post_server();
	/*timeout client remove for the client list*/
	for(list_pos=clt_head->next;list_pos!=clt_head;){
		pclt = list_entry(list_pos,struct clt_item, ci_list);
		struct list_head* next = list_pos->next;	//save the next node
		get_server();
		list_del(&pclt->ci_list);
		post_server();

		clt_release(pclt);
		free(pclt);
		if(next==clt_head){
			break;
		}else{
			list_pos = next;
		}
	}
}

/*test client whethe overtime*/
int clt_is_timeout(CLT_T* pclt){
	struct timeval curtm;
	if(pclt==NULL || pclt->ci_timeval.tv_sec==0){	//never timeout if the timmer interval is not set 
		return 0;
	}

	gettimeofday(&curtm, NULL);
	return (pclt->ci_timeval.tv_sec+pclt->ci_last_time.tv_sec < curtm.tv_sec? 1:0);
}

/*client initiate*/
void clt_init(CLT_T* pclt){
	sem_init(&pclt->ci_sem, 0, 1);
	INIT_LIST_HEAD(&pclt->ci_list);
}
/*release client*/
void clt_release(CLT_T* pclt){
	if(pclt==NULL || !sem_trywait(&pclt->ci_sem)){
		return;
	}
	sem_close(&pclt->ci_sem);
}

/*success return 0, else return -1*/
int clt_add(CLT_T* pclt){
	struct server_data* sd = get_server();
	struct list_head* plh = NULL;
	if(sd==NULL){
		return -1;
	}
	plh = &sd->sd_clt_head;
	
	list_add(&pclt->ci_list,plh);
	post_server();
}

#undef __CLT_ITEM_C__
