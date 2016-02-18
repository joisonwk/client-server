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



/*process method identical*/
typedef enum process_method_flag{
	PMF_ID_PARSE,
	PMF_MAX_METHOD,
}PM_FLAG;

/*process method running status*/
typedef enum process_method_stat{
	PMS_UNINIT,
	PMS_INITED,
} PM_STAT;

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


/*1.0 method api*/
/*1.1 method init*/
typedef int (*PM_INIT_FUNC) (void* conf);
/*1.2 method deal*/
typedef int (*PM_DEAL_FUNC) (PD_T* pd);
/*1.3 process method release*/
typedef void(*PM_EXIT_FUNC) (void);

typedef struct process_method{
	char pm_ctrl_file[MAX_PATH_LEN];
	PM_FLAG pm_flag; 	//method flags	
	PM_STAT pm_stat;	//method status
	struct timeval pm_rtv;	//start time
	PM_INIT_FUNC pm_init;
	PM_DEAL_FUNC pm_deal;	
	PM_EXIT_FUNC pm_exit;
	struct proces_method* next;
} PM_T;


static void method_fresh(void);

/*process init*/
PROCESS_COREEXTERN int process_init(void);
/*process deal*/
PROCESS_COREEXTERN int process_deal(PD_T* pd);
/*process method add*/
PROCESS_COREEXTERN void process_exit(void);

#endif //__PROCESS_CORE_H__
