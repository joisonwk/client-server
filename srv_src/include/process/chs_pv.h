/*src/process/chs_pv.h*/
#ifndef __CHS_PV_H__
#define __CHS_PV_H__
#include <stdio.h>

typedef struct province{
	unsigned int pv_id;
	char pv_nm[128];
	struct province* next;
}PV_T;

/*
*according the value of pv_id,converting id to province name stored to pv_nm, the province name length is limited by the argument @pv_len
*/
extern int id_to_province(unsigned int pv_id, char* pv_nm, ssize_t pv_len);

/*
*loading province info list from the file pointed to by pv_fnm */
extern int province_init(const char* pv_fnm);

/*
* release the province info list
*/
extern void province_release(void);
#endif 	//__CHS_PV_H__