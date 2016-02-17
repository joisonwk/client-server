#ifndef __PROCESS_H__
#define __PROCESS_H__
#ifndef __PROCESS_C__
#define PROCESS_EXTERN extern
#else
#define PROCESS_EXTERN 
#endif //__PROCESS_C__

typedef unsigned char PROCESS_METHOD_T;

typedef struct process_data{
	sem_t pd_sem;
	PROCESS_METHOD_T pd_head;	
	char pd_buf[MAX_CMD_LEN];
	unsigned int pd_len;
	struct process_data* next;
} PROCESS_DATA_T;

typedef int (*PROCESS_FUNC)(PROCESS_DATA_T* pdata);

typedef struct process_method{
	sem_t pd_sem;
	PROCESS_FUNC* pd_func;
} PROCESS_METHOD_T;

/*default process*/
PROCESS_EXTERN int id_process(const char* in,char* out);

/*process init*/
PROCESS_EXTERN int process_init(void);
/*add process method*/
PROCESS_EXTERN int process_method_add(PROCESS_DATA_T* pdata);


#endif //__PROCESS_H__
