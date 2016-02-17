/*src/process/chs_ct.c*/
#define __CHS_CT_C__
#include <process/chs_ct.h>

static CT_T* ct_head;
/*ct_id to city name*/
int id_to_city(unsigned int ct_id, char* ct_nm, unsigned int ct_len){
	int ct_exist = 0;

	CT_T ct_tmp = ct_head;
	while(ct_tmp){
		if(ct_tmp->ct_id == ct_id){
			snprintf(ct_nm, ct_len, ct_tmp->ct_nm);	
			ct_exist = 1;
		}
		ct_tmp = ct_tmp->next;
	}
	if(!ct_exist)
		snprintf(ct_nm, ct_len, "UNKNOW_CITY");	
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
		CT_T* pct = malloc(sizeof(CT_T));
		if(pct == NULL){
			perror("allocate city structure failed");
			return -1;
		}

		char id_buf[128];
		sscanf(strline, "%s=%s", id_buf, pct->ct_nm);
		pct->ct_id = atoi(id_buf);			
	}
	/*free the getline allocate strline memory*/
	if(strline){
		free(strline);
	}
}

void city_release(void){
	CT_T* index = NULL ;
	while(ct_head){
		index = ct_head;
		free(index);
		ct_head = ct_head->next;
	}
}

#undef __CHS_CT_C__
