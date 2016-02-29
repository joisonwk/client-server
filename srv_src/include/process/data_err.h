#ifndef __DATA_ERR_H__
#define __DATA_ERR_H__
#include <process/process_core.h>
#include <server/clt_item.h>

#ifndef __DATA_ERR_C__
#define DATA_ERR_EXTERN extern
#else
#define DATA_ERR_EXTERN 
#endif

struct data_err_data{
	unsigned int ded_errcmd_cnt;
	unsigned int ded_errdata_cnt;
};

static int data_err_init(void* pdata);
static void data_err_deal(CLT_T* pclt);
static void data_err_exit(void);

DATA_ERR_EXTERN void cmd_jump (CLT_T* pclt);
#endif //__DATA_ERR_H__
