/*src/process/id_chs.c*/
#define __ID_CHS_C__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <process/process_core.h>
#include <process/id_chs.h>
#include <process/chs_pv.h>
#include <process/chs_ct.h>
#include <process/chs_ctr.h>
#define ID_CTRL_FILES "pv.conf ct.conf ctr.conf"

/*
*parse the id_num to address, return the info length
* errno return -1
*/
void id_parse(const char* id_num, int id_len, char* retstr, int *retlen){
	printf("id_num=%s, id_len=%d\n", id_num, id_len);
	int ret, curlen = 0;
	char cdigit = 0;
	int pv_id, ct_id, ctr_id;
	char nmbuf[128];
	if((id_num==NULL) || id_len<=0){
		return;
	}

	printf("%s%d\n",__func__, __LINE__);
	ret = id_check(id_num, id_len);
	printf("%s%d\n",__func__, __LINE__);
	if(ret != 0){
		snprintf(retstr, *retlen,"%s:this ID is illegal!\n", id_num);
	}else{
		printf("ID is right\n");
	}

	curlen += id_to_province(id_num, retstr, *retlen-curlen);
	printf("PV=%s, curlen=%d\n", retstr, curlen);
	curlen += id_to_city(id_num+2, retstr+curlen, *retlen-curlen);
	printf("CT=%s\n", retstr);
	curlen += id_to_country(id_num+4, retstr+curlen, *retlen-curlen);
	*retlen = curlen;
	printf("id_address[%s]\n", retstr);
	return;	
}

/**/
void id_parse_deal(CLT_T* pclt){
	char id_buf[CHS_ID_LEN], addr_buf[CHS_ADDR_LEN];
	int id_len = CHS_ID_LEN;
	int addrlen = CHS_ADDR_LEN;
	
	if(pclt==NULL || sem_trywait(&pclt->ci_sem)){
		return; 
	}
	if(pclt->ci_rlen<CHS_ID_LEN){	//length is not enough return err message
		pclt->ci_rlen = 0;
		pclt->ci_wlen += snprintf(pclt->ci_wbuf+pclt->ci_wlen, 
			MAX_SEND_LEN-pclt->ci_wlen,
			"ERR:ID length is not enough\n");
		sem_post(&pclt->ci_sem);
		return;
	}else if(pclt->ci_rlen >= CHS_ID_LEN){
		pclt->ci_rlen -= CHS_ID_LEN;	
		memcpy(id_buf, pclt->ci_rbuf,CHS_ID_LEN);
		if(pclt->ci_rlen>0)
			memcpy(pclt->ci_rbuf, pclt->ci_rbuf+CHS_ID_LEN, pclt->ci_rlen);	//move dealed data
		sem_post(&pclt->ci_sem);
		id_parse(id_buf, CHS_ID_LEN, addr_buf, &addrlen);	
		int i;
		for(i=10;i>0;i--){
			if(!sem_trywait(&pclt->ci_sem)){
				int wleft= MAX_SEND_LEN-pclt->ci_wlen;
				int cplen = wleft<addrlen? wleft:addrlen;
				memcpy(pclt->ci_wbuf+pclt->ci_wlen,addr_buf, cplen);
				pclt->ci_wlen += cplen;
				printf("[send] [%s]\n", pclt->ci_wbuf);
				sem_post(&pclt->ci_sem);
				break;
			}else{
				usleep(1000);	
			}
		}
	}

	return;
}



/*check the id_num ascll value, and the certifiation val*/
int id_check(const char* id_num, unsigned int id_len){
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

/*initiate success return 0, else return -1*/
int id_parse_init(void* pm_ctr){
	printf("id parse init start\n");
	int ret = -1;
	char pv_fn[128] = {0};
	char ct_fn[128] = {0};
	char ctr_fn[128] = {0};
	
	int off_set = 0;
	sscanf(pm_ctr, "%s %s %s", pv_fn, ct_fn, ctr_fn);
	printf("pv_filename=%s, ct_filename=%s, ctr_filename=%s\n", pv_fn , ct_fn, ctr_fn);
	pv_fn[strlen(pv_fn)]=0;
	ct_fn[strlen(ct_fn)]=0;
	ctr_fn[strlen(ctr_fn)]=0;
	
	/*province info initiate*/	
	if(province_init(pv_fn)){
		printf("id province initiate failed\n");
		goto err_province_init;
	}
	/*city info initiate*/
	if(city_init(ct_fn)){
		printf("id city initiate failed\n");	
		goto err_city_init;
	}
	/*country info initiate*/
	if(country_init(ctr_fn)){
		printf("id country initiate failed\n");
		goto err_country_init;
	}
	
	printf("id process method initiate success...");
	return 0;
	
err_country_init:
	city_release();
err_city_init:
	province_release();
err_province_init:
	printf("id process method initiate failed...");
	return -1;
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
	.pm_ctrl_file = "pv.conf ct.conf ctr.conf", 
	.pm_header = "IDTOADDR",
	.pm_timeval.tv_sec = 0,
	.pm_stat = PMS_UNINIT, 
	.pm_init = (PM_INIT)id_parse_init,
	.pm_deal = id_parse_deal, 
	.pm_exit = id_parse_exit, 
};

PM_T* get_id_process_method(void){
	printf("conf = %s, header=%s\n", id_process_method.pm_ctrl_file, id_process_method.pm_header);
	return &id_process_method;
}

#undef __ID_CHS_C__
