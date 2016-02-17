/*include/process/id_chs.h*/
#ifndef __ID_CHS_H__
#define __ID_CHS_H__

#ifdef __ID_CHS_C__
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

ID_CHS_EXTERN int id_parse_init(void);
ID_CHS_EXTERN int id_parse(const char* id_num, int id_len, char* id_info);
ID_CHS_EXTERN void id_parse_exit(void);

#endif //__ID_CHS_H__
