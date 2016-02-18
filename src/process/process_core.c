/*src/process/process_core.c*/
#define __PROCESS_CORE_C__
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <semaphore.h>
#include <process/process_core.h>
#include <process/id_chs.h>	//id process method

/*include the process method valibles*/
extern struct PM_T id_process_method;

static PM_T* ppm_tab[PMF_MAX_METHOD] = {
	&id_process_method,
};


/**/
int process_init(void){
	int i;
	for(i=0;i<PMF_MAX_METHOD;i++){
		if(!ppm_tab[i]->pm_init(ppm_tab[i].pm_ctrl_file)){
			ppm_tab[i]->pm_stat = PMS_INITED;
			ppm_tab[i]->next = NULL;
			gettimeofday(&ppm_tab[i].pm_rtv,NULL);
		}
	}
	return 0;
}

/*deal success return 0, else return -1*/
int process_deal(PD_T* pd){
	int i;	
	sem_wait(&pd->pd_sem);
	PM_FLAG pm_flag= pd->pd_pmf;
	if(pd == NULL){
		return -1;
	}

	/*match method*/
	for(i=0;i<PMF_MAX_METHOD;i++){
		if(pm_flag == ppm_tab[i]->pm_flag){
			if(ppm_tab[i]->pm_stat == PMS_UNINIT){
				if(ppm_tab[i]->pm_init)	
					ppm_tab[i]->pm_init(ppm_tab[i]->pm_ctrl_file);
				ppm_tab[i]->pm_stat = PMS_INITED;
			}

			if(ppm_tab[i]->pm_deal){
				pd->pd_st = ppm_tab[i]->pm_deal(pd);
			}

			ppm_tab[i]->next = NULL;
			gettimeofday(&ppm_tab[i]->pm_rtv,NULL);
		}
	}

	sem_post(&pd->pd_sem);
	return 0;
}

/*run method refresh*/
void method_fresh(void){
	int i;
	struct timeval tm;
	gettimeofday(&tm,NULL);
	for(i=0;i<PMF_MAX_METHOD;i++){
		if(tm.tv_sec > ((ppm_tab[i]->pm_rtv.tv_sec)+METHOD_TIMEOUT)){
			if(ppm_tab[i]->pm_exit){
				ppm_tab[i]->pm_exit();
				ppm_tab[i]->pm_stat = PMS_UNINIT;
			}
		}
	}
}

void process_exit(void){
	int i;
	for(i=0;i<PMF_MAX_METHOD;i++){
		if(ppm_tab[i]->pm_stat == PMS_INITED && ppm_tab[i]->pm_exit){
			ppm_tab[i]->pm_exit();
			ppm_tab[i]->pm_stat = PMS_UNINIT;
		}
	}
}

#undef __PROCESS_CORE_C__
