/*include/process/id_chs.h*/
#ifndef __ID_CHS_H__
#define __ID_CHS_H__

#include <stdio.h>
#include <server/process_core.h>

#ifndef __ID_CHS_C__
#define ID_CHS_EXTERN 	extern
#else
#define ID_CHS_EXTERN
#endif

#define CHS_ID_LEN	18
#define CHS_ADDR_LEN	512
#define DATE_LEN	8

struct id_data {
	char id_num[CHS_ID_LEN];
	char id_ret[CHS_ADDR_LEN];
};

static int id_lastnum_chk_ok(const char* id_num);
static int id_check(const char* id_num);

static int id_parse_init(void* id_conf);
static int id_parse_deal(CLT_T* pd);
static void id_parse_exit(void);

#endif //__ID_CHS_H__
