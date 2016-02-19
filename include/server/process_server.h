/*include/server/process_core.h*/
#ifndef __PROCESS_CORE_H__
#define __PROCESS_CORE_H__
#include<server/server_core.h>
#include <util/c_list.h>
#include <util/c_kernel.h>
#include <server/clt_item.h>

#define CMD_MAXLEN 512
#define CMD_HEAD_MAXLEN 20

/*cmd head string define*/
#define ID_TO_ADDR "IDTOADDR"
#define ADDR_TO_ID "ADDRTOID"

/*process method running status*/
enum method_stat{
	MS_UNINIT,
	MS_INITED,
};


/*1.0 method api*/
/*1.1 method init*/
typedef int (*PM_INIT_FUNC) (void* conf);
/*1.2 method deal*/
typedef int (*PM_DEAL_FUNC) (CLT_T* pclt);
/*1.3 process method release*/
typedef void(*PM_EXIT_FUNC) (void);

typedef struct process_method{
	sem_t pm_sem;
	enum method_stat pm_stat;	//method stat
	char pm_ctrl[MAX_PATH_LEN];
	char pm_header[CMD_HEAD_MAXLEN];
	struct timeval pm_rtv;	//start time
	struct timeval pm_last_tm;
	#define METHOD_TIME	600	//10mins timeout
	PM_INIT_FUNC pm_init;
	PM_DEAL_FUNC pm_deal;	
	PM_EXIT_FUNC pm_exit;
	struct list_head pm_list;
} PM_T;

/*static func declare*/
static int process_data(CLT_T* pclt);

static int method_fresh(PM_T* ppm);
/*process thread*/
extern int process_thread(void* pdata);
extern void process_flush(void);
extern void process_exit(void);

#endif //__PROCESS_CORE_H__
