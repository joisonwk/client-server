/*include/process/id_chs.h*/
#ifndef __ID_CHS_H__
#define __ID_CHS_H__

#include <stdio.h>
#include <process/process_core.h>

#ifndef __ID_CHS_C__
#define ID_CHS_EXTERN 	extern
#else
#define ID_CHS_EXTERN
#endif

#define CHS_ID_LEN	18
#define CHS_ADDR_LEN	512
#define DATE_LEN	8

struct id_info {
	char ii_num[CHS_ID_LEN];
	char ii_buf[CHS_ADDR_LEN];
};

static int id_parse_init(const char* pm_ctrl);
static enum pd_stat id_parse(const char* id_num, int id_len, char* id_info, ssize_t* retlen);
static int id_parse_deal(PD_T* pd);
static int id_check(const char* id_num, int id_len);
static void id_parse_exit(void);
static int id_lastnum_chk_ok(const char* id_num);

ID_CHS_EXTERN PM_T id_process_method;
#endif //__ID_CHS_H__
