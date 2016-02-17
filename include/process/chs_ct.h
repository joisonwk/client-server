/*src/process/chs_pv.h*/
#ifndef __CHS_CT_H__
#define __CHS_CT_H__

typedef struct city{
	unsigned int ct_id;		//city id
	char ct_nm[128];	//city name 
	struct city* next;	//when city retrieve
}CT_T;

/*
*according the value of ct_id,converting id to city name stored to ct_nm, the city name length is limited by the argument @ct_len
*/
extern int id_to_city(unsigned int ct_id, char* ct_nm, unsigned int ct_len);

/*
*loading city info list from the file pointed to by ct_fnm */
extern int city_init(const char* ct_fnm);

/*
* release the city info list
*/
extern void city_release(void);
#endif 	//__CHS_CT_H__
