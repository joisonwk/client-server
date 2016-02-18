/*src/process/chs_ctr.h*/
#ifndef __CHS_CTR_H__
#define __CHS_CTR_H__
#include <stdio.h>

typedef struct country{
	unsigned int ctr_id;
	char ctr_nm[128];
	struct country* next;
}CTR_T;

/*
*according the value of ctr_id,converting id to country name stored to ctr_nm, the country name length is limited by the argument @ctr_len
*/
extern int id_to_country(unsigned int ctr_id, char* ctr_nm, ssize_t ctr_len);

/*
*loading country info list from the file pointed to by ctr_fnm */
extern int country_init(const char* ctr_fnm);

/*
* release the country info list
*/
extern void country_release(void);
#endif 	//__CHS_CTR_H__
