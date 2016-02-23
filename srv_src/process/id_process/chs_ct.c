/*src/process/chs_ct.c*/
#define __CHS_CT_C__
#include <process/chs_ct.h>
#include <stdio.h>

static CT_T* ct_head;
/*ct_id to city name*/
int id_to_city(unsigned int ct_id, char* ct_nm, ssize_t ct_len){
	int cnlen = 0;
	int ct_exist = 0;

	CT_T* ct_tmp = ct_head;
	while(ct_tmp){
		if(ct_tmp->ct_id == ct_id){
			cnlen += snprintf(ct_nm, ct_len, ct_tmp->ct_nm);	
			ct_exist = 1;
		}
		ct_tmp = ct_tmp->next;
	}
	if(!ct_exist)
		cnlen += snprintf(ct_nm, ct_len, "UNKNOW_CITY");	
	return cnlen;
}

int city_init(const char* ct_fnm){
	FILE* fp = NULL;
	char* strline = NULL;
	char ct_nm[128];
	if(ct_fnm == NULL){
		printf("city file name is empty\n");
	}

	/*open city file*/
	fp = fopen(ct_fnm, "r");
	if(fp == NULL){
		char errinfo[128];
		sprintf(errinfo, "open [%s] failed", ct_fnm);
		perror(errinfo);	
		return -1;
	}

	/*loading city info to city list*/
	while(getline(&strline, NULL, fp)){
		CT_T* pct = (CT_T*)malloc(sizeof(CT_T));
		if(pct == NULL){
			perror("allocate city structure failed");
			return -1;
		}
		bzero(pct, sizeof(CT_T));
		
		char id_buf[128];
		sscanf(strline, "%s=%s", id_buf, pct->ct_nm);
		pct->ct_id = atoi(id_buf);			
	}
	/*free the getline allocate strline memory*/
	if(strline){
		free(strline);
	}
	printf("loading city information from %s success\n", ct_fnm);
}

void city_release(void){
	CT_T* index = ct_head;
	while(index){
		ct_head = index->next;
		free(index);
		index = ct_head;
	}
}

#undef __CHS_CT_C__
