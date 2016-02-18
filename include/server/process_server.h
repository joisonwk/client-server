/*include/server/process_server.h*/
#ifndef __PROCESS_SERVER_H__
#define __PROCESS_SERVER_H__
#include<server/server_core.h>

#define CMD_MAXLEN 512
#define CMD_HEAD_MAXLEN 20

/*cmd head string define*/
#define ID_TO_ADDR "IDTOADDR"
#define ADDR_TO_ID "ADDRTOID"

typedef struct process_data{
	int pd_fd;
	char pd_ibuf[CMD_MAX_LEN];
	ssize_t pd_ilen;
	char pd_obuf[CMD_MAX_LEN];
	ssize_t pd_olen;	
	struct process_data* prew;
	struct process_data* next;
} PD_T;

/*process method running status*/
typedef enum process_method_stat{
	PMS_UNINIT,
	PMS_INITED,
} PM_STAT;


/*1.0 method api*/
/*1.1 method init*/
typedef int (*PM_INIT_FUNC) (void* conf);
/*1.2 method deal*/
typedef int (*PM_DEAL_FUNC) (PD_T* pd, size_t hlen);
/*1.3 process method release*/
typedef void(*PM_EXIT_FUNC) (void);

typedef struct process_method{
	char pm_ctrl_file[MAX_PATH_LEN];
	sem_t pm_sem;
	char pm_header[CMD_HEAD_MAXLEN];
	struct timeval pm_rtv;	//start time
	#define METHOD_TIME	600	//10mins timeout
	PM_INIT_FUNC pm_init;
	PM_DEAL_FUNC pm_deal;	
	PM_EXIT_FUNC pm_exit;
} PM_T;

/*static func declare*/
static process_data(PD_T* pd);
static process_exit(void);

/*process thread*/
extern int process_thread(void* pdata);
extern void process_flush(void);
extern void process_exit(void);

#endif //__PROCESS_SERVER_H__
