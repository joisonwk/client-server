/*include/process/process_core.h*/
#ifndef __PROCESS_CORE_H__
#define __PROCESS_CORE_H__
#include <semaphore.h>
#include <common.h>
#include <server/clt_item.h>

enum pm_stat {
	PMS_UNINIT,
	PMS_INITED,	
};

typedef int (*PM_INIT)(void* path);
typedef void (*PM_DEAL)(CLT_T* pclt);
typedef void (*PM_EXIT)(void);

typedef struct process_method{
	char pm_ctrl_file[512];
	unsigned char pm_header[20];
	enum pm_stat pm_stat;
	struct timeval pm_lasttime;	
	struct timeval pm_timeval;	//if not set the method will not be released
	PM_INIT pm_init;
	PM_DEAL pm_deal;
	PM_EXIT pm_exit;
}PM_T;

/*update the method time*/
static void method_fresh(PM_T* ppm);
/*remove timeout process method*/
extern void process_flush(void);

/*process init*/
static int process_init(void);
/*process deal*/
static int process_deal(CLT_T* pclt);
/*process method add*/
static void process_exit(void);

extern void* process_thread(void* pdata);

#endif //__PROCESS_CORE_H__
