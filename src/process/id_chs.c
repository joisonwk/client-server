/*src/process/id_chs.c*/
#define __ID_CHS_C__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <process/process_core.h>
#include <process/id_chs.h>
#include <process/chs_pv.h>
#define ID_CTRL_FILES "pv.conf ct.conf ctr.conf"

/*
*parse the id_num to id info, return the info length
* errno return -1
*/
enum pd_stat id_parse(const char* id_num, ssize_t id_len, char* retstr, ssize_t*retlen){
	int ret, curlen;
	char cdigit = 0;
	int pv_id, ct_id, ctr_id;
	char nmbuf[128];
	struct id_info id_info;
	if((id_num==NULL) || id_len<=0){
		return PS_ERR;
	}

	ret = id_check(id_num, id_len);
	if(ret != 0){
		snprintf(retstr, *retlen,"%s:this ID is illegal!\n", id_num);
	}

	pv_id = (id_num[0]-'0')* 10 + (id_num[1]-'0');	
	ct_id = (id_num[2]-'0')* 10 + (id_num[3]-'0');	
	ctr_id = (id_num[4]-'0')* 10 + (id_num[5]-'0');	
	curlen += id_to_province(pv_id, retstr, *retlen-curlen);
	curlen += id_to_city(ct_id, retstr+curlen, *retlen-curlen);
	curlen += id_to_country(ctr_id, retstr+curlen, *retlen-curlen);
	*retlen = curlen;
	return PS_SUCCESS;	
}

int id_parse_deal(PD_T* pd){
	pd->pd_st = id_parse(pd->pd_ibuf, pd->pd_ilen, pd->pd_obuf, &pd->pd_olen);	
	return 0;
}



/*check the id_num ascll value, and the certifiation val*/
int id_check(const char* id_num, int id_len){
	if(id_len!=CHS_ID_LEN){
		return -1;
	}
		
	int i = 0;
	for(i=0; i<17; i++){
		if(!isdigit(id_num[i])){
			return -1;
		}
	}

	if(!id_lastnum_chk_ok(id_num)){
		return -1;
	}
}

/*when the lastnum is right return true 1*/
int id_lastnum_chk_ok(const char* id_num){
	/*uncomplete:*/
	return 1;
}

/*id parse initiate*/
int id_parse_init(const char* pm_ctr){
	int ret = -1;
	char pv_fn[128] = {0};
	char ct_fn[128] = {0};
	char ctr_fn[128] = {0};
	
	ssize_t off_set = 0;
	off_set += snprintf(pv_fn, 128, pm_ctr);
	off_set += snprintf(ct_fn, 128, pm_ctr+off_set);
	snprintf(ctr_fn, 128, pm_ctr+off_set);
	
	/*province info initiate*/	
	if(province_init(pv_fn)){
		printf("id province initiate failed\n");
		return -1;
	}
	/*city info initiate*/
	if(city_init(ct_fn)){
		printf("id city initiate failed\n");	
		return -1;
	}
	/*country info initiate*/
	if(country_init(ctr_fn)){
		printf("id country initiate failed\n");
		return -1;
	}

	return 0;
}

/*id parse exit*/
void id_parse_exit(void){
	/*province release*/
	province_release();
	/*city release*/
	city_release();
	/*country release*/
	country_release();
}


PM_T id_process_method = {
	.pm_ctrl_file = ID_CTRL_FILES, 
	.pm_flag = PMF_ID_PARSE, 
	.pm_stat = PMS_UNINIT, 
	.pm_init = id_parse_init,
	.pm_deal = id_parse_deal, 
	.pm_exit = id_parse_exit, 
	.next = NULL
};

#undef __ID_CHS_C__
