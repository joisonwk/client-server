/*src/process/chs_pv.h*/
#ifndef __CHS_PV_H__
#define __CHS_PV_H__
#include <stdio.h>

typedef struct province{
	char pv_id[3];
	char pv_nm[128];
	struct province* next;
}PV_T;

/*
*according the value of pv_id,converting id to province name stored to pv_nm, the province name length is limited by the argument @pv_len
*/
extern int id_to_province(const char* pv_id, char* pv_nm, int pv_len);

/*
*loading province info list from the file pointed to by pv_fnm */
extern int province_init(const char* pv_fnm);

/*
* release the province info list
*/
extern void province_release(void);
#endif 	//__CHS_PV_H__
