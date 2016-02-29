/*srv_src/process/statistics/count.c*/
#define __COUNT_C__
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <process/process_core.h>
#include <process/count.h>

static struct count_data* pcd = NULL;
	
struct process_method count_method = {
	.pm_stat = PMS_UNINIT,
	.pm_init = count_init,
	.pm_deal = count_deal,
	.pm_exit = count_exit,
};

/*count each client recive data*/
void count_deal(CLT_T* pclt){
	static int llsize = sizeof(unsigned long long);//record the long data length
	if(NULL == pclt || NULL == pcd){
		return;
	}
	
	/*when the data is over the scaler show the result*/
	if(((pcd->cd_ovs+1)==(1<<sizeof(unsigned int))) &&(pcd->cd_tcp_bytes+pclt->ci_rlen>=(1<<llsize))
	){
		count_show();
	}
	if(sem_wait(&pcd->cd_sem))
		return;

	if(pcd->cd_tcp_bytes+pclt->ci_rlen>=(1<<llsize)){
		pcd->cd_tcp_bytes = pcd->cd_tcp_bytes + 
			pclt->ci_rlen - (1<<llsize);
		pcd->cd_ovs++;
	}else{
		pcd->cd_tcp_bytes += pclt->ci_rlen;	
	}
	sem_post(&pcd->cd_sem);
}

int count_init(void* pdata){
	if(pcd == NULL){
		pcd = malloc(sizeof(struct count_data));
		if(pcd == NULL){
			perror("count init failed");
			return -1;
		}
		bzero(pcd , sizeof(struct count_data));
	}
	
	sem_init(&pcd->cd_sem, 0, 1);
	gettimeofday(&pcd->cd_stm, NULL);
	return 0;
}

void count_exit(void){
	if(pcd){
		sem_close(&pcd->cd_sem);
		free(pcd);
		pcd = NULL;
	}
}

void count_show(void){
	struct timeval cur_tm;
	if(NULL == pcd){
		return;
	}

	gettimeofday(&cur_tm, NULL);
	printf("start time:%s\n", ctime(&pcd->cd_stm.tv_sec));
	printf("current time:%s\n",ctime(&cur_tm.tv_sec));
	printf("all bytes: %u*%llu+%llu\n", pcd->cd_ovs, 
		(1<<sizeof(unsigned long long int)),pcd->cd_tcp_bytes);
	//clear
	pcd->cd_tcp_bytes = 0;
	pcd->cd_ovs = 0;

	if(!sem_wait(&pcd->cd_sem)){
		gettimeofday(&pcd->cd_stm,NULL);//reset start time
		sem_post(&pcd->cd_sem);
	}
}
#undef __COUNT_C__
