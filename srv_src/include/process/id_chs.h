/*include/process/id_chs.h*/
#ifndef __ID_CHS_H__
#define __ID_CHS_H__

#include <stdio.h>
#include <server/clt_item.h>
#include <process/process_core.h>

#define CHS_ID_LEN	18
#define CHS_ADDR_LEN	512
#define DATE_LEN	8

struct id_data {
	char id_num[CHS_ID_LEN];
	char id_ret[CHS_ADDR_LEN];
};

static void id_parse(const char* id_num, int id_len, char* retstr, int *retlen);

static int id_lastnum_chk_ok(const char* id_num);

static int id_check(const char* id_num, unsigned int idlen);

static int id_parse_init(void* id_conf);
static void id_parse_deal(CLT_T* pclt, struct cmd_info* pcmd);
static void id_parse_exit(void);

extern PM_T* get_id_process_method(void);

#endif //__ID_CHS_H__
