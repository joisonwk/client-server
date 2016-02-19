/*src/process/process_core.c*/
#define __PROCESS_CORE_C__
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <util/c_list.h>
#include <server/proces_core.h>

#define MAX_METHOD	20

/*include the process method valibles*/
extern struct PM_T id_process_method;
static PM_T* ppm_tab[MAX_METHOD] = {
	&id_process_method,
};


/**/
int process_init(void){
	int i;
	for(i=0;i<MAX_METHOD;i++){
		if(NULL==ppm_tab[i]){
			continue;
		}
		sem_init(&ppm_tab[i]->pm_sem,0,1);
		if(!ppm_tab[i]->pm_init(ppm_tab[i].pm_ctrl_file)){
			ppm_tab[i]->pm_stat = PMS_INITED;
			gettimeofday(&ppm_tab[i]->pm_rtv,NULL);
		}
	}
	return 0;
}

/*deal success return 0, else return -1*/
int process_deal(PD_T* pd){
	int i;	
	if(pd == NULL){
		return -1;
	}

	/*match method*/
	for(i=0;i<MAX_METHOD;i++){
		if(NULL == ppm_tab[i])
			continue;
		
		size_t hlen = strlen(ppm_tab[i]->pd_header);
		if(!strncmp(ppm->pm_header,pd->pd_ibuf)){
			/*update the method trigger time*/
			gettimeofday(&ppm_tab[i]->pm_rtv,NULL);

			if(ppm_tab[i]->pm_stat == PMS_UNINIT){
				if(ppm_tab[i]->pm_init)	
					ppm_tab[i]->pm_init(ppm_tab[i]->pm_ctrl_file);
				ppm_tab[i]->pm_stat = PMS_INITED;
			}

			if(ppm_tab[i]->pm_deal){
				if(!ppm_tab[i]->pm_deal(pd, hlen)){
					write(pd->pd_fd,pd->pd_obuf,pd->pd_olen);
				}
			}
		}
	}
	return 0;
}

/*process thread func*/
void* process_thread(void* pdata){
	struct server_data* psd = NULL; 
	struct CLT_T* pclt = NULL;
	struct list_head* pclt_head = NULL;
	if(pdata == NULL){
		pthread_exit(NULL);
	}
	psd = (struct server_data*) pdata;
	pclt_head = &psd->sd_clt_head;
	while(1){
		list_for_each_entry(pclt,psd->sd_clt_head,ci_list){
			if(!sem_trywait(pclt->ci_sem)){
				if(pclt->ci_rlen>0){
					process_deal(pclt);
					clt_fresh(pclt);
					if(pclt->ci_wlen>0){
						clt_send(pclt);	
					}
				}
				sem_post(&pclt->ci_sem);	
			}
		}
	}
}

/*run process refresh*/
void process_flush(void){
	int i;
	struct timeval cur_tm;
	gettimeofday(&cur_tm,NULL);
	for(i=0;i<MAX_METHOD;i++){
		if(ppm_tab[i] == NULL)
			continue;
		if(cur_tm.tv_sec > ((ppm_tab[i]->pm_rtv.tv_sec)+METHOD_TIMEOUT)){
			if(ppm_tab[i]->pm_exit){
				ppm_tab[i]->pm_exit();
				ppm_tab[i]->pm_stat = PMS_UNINIT;
			}
		}
	}
}

void process_exit(void){
	int i;
	for(i=0;i<MAX_METHOD;i++){
		if(ppm_tab[i] == NULL)
			continue;
		if(ppm_tab[i]->pm_stat == PMS_INITED && ppm_tab[i]->pm_exit){
			ppm_tab[i]->pm_exit();
			ppm_tab[i]->pm_stat = PMS_UNINIT;
		}
	}
}

#undef __PROCESS_SERVER_C__
