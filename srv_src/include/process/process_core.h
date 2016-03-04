/*include/process/process_core.h*/
#ifndef __PROCESS_CORE_H__
#define __PROCESS_CORE_H__
#include <semaphore.h>
#include <common.h>
#include <server/clt_item.h>
#include <util/c_list.h>
#define MAX_CMD_HEADER_LEN 20

struct cmd_info {
	char ci_header[MAX_CMD_HEADER_LEN];	//cmd header
	int ci_pm_pos;	//cmd process method index
	char* ci_offset;	//cmd recive buf offset
	unsigned int ci_left;	//left data
	unsigned ci_ret; //cmd deal result
	struct list_head ci_list;
};

enum pm_stat {
	PMS_UNINIT,
	PMS_INITED,	
};

typedef int (*PM_INIT)(void* path);
typedef void (*PM_DEAL)(CLT_T* pclt, struct cmd_info* pcmd_info);
typedef void (*PM_EXIT)(void);

typedef struct process_method{
	char pm_ctrl_file[512];
	unsigned char pm_header[20];
	enum pm_stat pm_stat;
	struct timeval pm_lasttime;	
	struct timeval pm_timeval;	//if not set the method will not be released
	struct cmd_info* pm_pcmd;
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

/*according the cmd data selecte valid cmd and match the method*/
static int cmd_select(CLT_T* pclt, struct list_head* pcmd_head);

extern void* process_thread(void* pdata);

#endif //__PROCESS_CORE_H__
