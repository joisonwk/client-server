#ifndef __PROCESS_H__
#define __PROCESS_H__
#ifndef __PROCESS_C__
#define PROCESS_EXTERN extern
#else
#define PROCESS_EXTERN 
#endif //__PROCESS_C__

typedef unsigned char PROCESS_METHOD_T;

typedef int (*PROCESS_FUNC)(PROCESS_DATA_T* pdata);

/*default process*/
PROCESS_EXTERN int id_process(const char* in,char* out);

/*process init*/
static int process_init(void);

#endif //__PROCESS_H__
