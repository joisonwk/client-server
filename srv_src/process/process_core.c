/*src/process/process_core.c*/
#define __PROCESS_CORE_C__
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <server/clt_item.h>
#include <server/server_core.h>
#include <util/c_list.h>
#include <process/id_chs.h>
#include <process/process_core.h>

#define MAX_METHOD	20
extern PM_T id_process_method;
static PM_T* ppm_tab[MAX_METHOD] = {
	&id_process_method,
};


/**/
int process_init(void){
	int i;
	struct server_data* psd = NULL;
	psd = get_server();	
	post_server();

	for(i=0;i<MAX_METHOD;i++){
		PM_T* ppm = ppm_tab[i];
		struct cmd_info *pci = NULL;
		if(NULL==ppm){
			continue;
		}
		/*cmd header init*/
		if(ppm->pm_header != NULL){
			pci = malloc(sizeof(struct cmd_info));			
			if(NULL==pci){
				perror("init cmd head failed");
				return -1;
			}
			bzero(pci, sizeof(struct cmd_info));
			LIST_HEAD_INIT(pci->ci_list);
			memcpy(pci->ci_header,ppm->pm_header, strlen(ppm->pm_header));
			list_add(&pci->ci_list, &psd->sd_cmdinfo_head);
			ppm->pm_pcmd = pci;	
		}
		if(ppm->pm_init){
			if(!ppm->pm_init(ppm->pm_ctrl_file))
				ppm->pm_stat = PMS_INITED;
			method_fresh(ppm);
		}
	}
	return 0;
}

/*if dealed return 0, else return -1*/
int process_deal(CLT_T* pclt){
	if(pclt==NULL){
		return 0;	//no data to deal,return dealed flag
	}
	/*match method*/
	int i;	
	for(i=0;i<MAX_METHOD;i++){
		PM_T* ppm = ppm_tab[i];
		if(NULL == ppm)
			continue;
		
		size_t hlen = strlen(ppm->pm_header);
		if(!strncmp(ppm->pm_header,pclt->ci_rbuf,hlen)){
			method_fresh(ppm);
			if(ppm->pm_init && ppm->pm_stat==PMS_UNINIT){
				ppm->pm_init(ppm->pm_ctrl_file);
				ppm_tab[i]->pm_stat = PMS_INITED;
			}

			if(ppm->pm_deal){
				if(!sem_trywait(&pclt->ci_sem)){
					printf("[%s][%s]\n",__func__,ppm->pm_header);
					pclt->ci_rlen -= hlen;
					memcpy(pclt->ci_rbuf,pclt->ci_rbuf+hlen,pclt->ci_rlen);
					sem_post(&pclt->ci_sem);
					printf("[process_deal][ID][%s]\n",pclt->ci_rbuf);
					ppm->pm_deal(pclt);
					return 0;
				}
			}
		}
	}

	if(!sem_trywait(&pclt->ci_sem)){
		sprintf(pclt->ci_wbuf, "ERROR DATA");	
		pclt->ci_wlen = strlen(pclt->ci_wbuf);
		sem_post(&pclt->ci_sem);
	}

	return -1;
}

/*process thread func*/
void* process_thread(void* pdata){
	struct server_data* psd = NULL; 
	CLT_T* pclt = NULL;
	struct list_head* pclt_head = NULL;

	psd = get_server();
	if(psd==NULL){
		pthread_exit(NULL);	
	}
	pclt_head = &psd->sd_clt_head;
	post_server();
	process_init();
	while(1){
		list_for_each_entry(pclt,pclt_head,ci_list){
			if(pclt->ci_rlen>0){
				process_deal(pclt);
				if(!sem_trywait(&pclt->ci_sem)){
					pclt->ci_rlen = 0;	
					sem_post(&pclt->ci_sem);
				}
				clt_fresh(pclt);
			}
		}
	}
	pthread_exit(NULL);
}

/*if timeout return 1, else return 0*/
int method_is_timeout(PM_T* ppm){
	struct timeval curtm;
	if(ppm==NULL || ppm->pm_timeval.tv_sec==0){
		return 0;
	}
	gettimeofday(&curtm,NULL);

	if(curtm.tv_sec > ppm->pm_lasttime.tv_sec+ppm->pm_timeval.tv_sec){
		return 1;
	}
	return 0;
}

/*update the process method lasttime*/
void method_fresh(PM_T* ppm){
	if(ppm==NULL || gettimeofday(&ppm->pm_lasttime,NULL)){}
}
/*run process refresh*/
void process_flush(void){
	int i;
	PM_T* ppm = NULL;
	for(i=0;i<MAX_METHOD;i++){
		ppm=ppm_tab[i];
		if(ppm == NULL)
			continue;
			if(ppm->pm_exit && method_is_timeout(ppm)){
				ppm->pm_exit();
				ppm->pm_stat = PMS_UNINIT;
			}
	}
}

void process_exit(void){
	int i;
	struct cmd_info* pcmd = NULL;
	for(i=0;i<MAX_METHOD;i++){
		if(ppm_tab[i] == NULL)
			continue;
		/*release the cmd*/
		pcmd = ppm_tab[i]->pm_pcmd;
		if(pcmd){
			list_del(&pcmd->ci_list);
			free(pcmd);
		}

		if(ppm_tab[i]->pm_stat == PMS_INITED && ppm_tab[i]->pm_exit){
			ppm_tab[i]->pm_exit();
			ppm_tab[i]->pm_stat = PMS_UNINIT;
		}
	}
}

/**/
int cmd_select(CLT_T* pclt){
	strspn();
}

#undef __PROCESS_SERVER_C__
