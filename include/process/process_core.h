/*include/process/process_core.h*/
#ifndef __PROCESS_CORE_H__
#define __PROCESS_CORE_H__
#ifndef __PROCESS_CORE_C__
#define PROCESS_COREEXTERN extern
#else
#define PROCESS_COREEXTERN 
#endif //__PROCESS_COREC__

#include <semaphore.h>
#include <common.h>

/*process data deal status*/
enum pd_stat{
	PS_UNKNOW,
	PS_SUCCESS,
	PS_ERR,
};

/*process data struct*/
typedef struct process_data{
	PM_FLAG	pd_pmf;	//process method flag
	enum pd_stat pd_st;
	char pd_ibuf[MAX_PDIBUF_LEN];
	ssize_t pd_ilen;
	char pd_obuf[MAX_PDOBUF_LEN];
	ssize_t pd_olen;
	struct process_data* prew;
	struct process_data* next;
}PD_T;


static void method_fresh(void);

/*process init*/
PROCESS_COREEXTERN int process_init(void);
/*process deal*/
PROCESS_COREEXTERN int process_deal(PD_T* pd);
/*process method add*/
PROCESS_COREEXTERN void process_exit(void);

#endif //__PROCESS_CORE_H__
