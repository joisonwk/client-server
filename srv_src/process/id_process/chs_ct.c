/*src/process/chs_ct.c*/
#define __CHS_CT_C__
#include <process/chs_ct.h>
#include <stdio.h>
#include <string.h>

static CT_T* pct_head = NULL;
/*ct_id to city name*/
int id_to_city(const char* ct_id, char* ct_nm, int ct_len){
	int cnlen = 0;
	
	printf("%s%d", __func__, __LINE__);
	CT_T* ct_tmp = pct_head;
	while(NULL != ct_tmp){
		if(!strncmp(ct_tmp->ct_id, ct_id,2)){
			cnlen += snprintf(ct_nm, ct_len, ct_tmp->ct_nm);	
			break;
		}
		ct_tmp = ct_tmp->next;
	}
	if(cnlen==0)
		cnlen += snprintf(ct_nm, ct_len, "UNKNOW_CITY");	
	return cnlen;
}

int city_init(const char* ct_fnm){
	printf("ct_fnm=%s\n", ct_fnm);
	FILE* fp = NULL;
	char* strline = NULL;
	int llen = 0;
	char ct_nm[128];
	CT_T* ct_index = NULL;
	if(ct_fnm == NULL){
		printf("city file name is empty\n");
		return -1;
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
	while(getline(&strline, &llen, fp)!=-1){
		CT_T* pct = (CT_T*)malloc(sizeof(CT_T));
		if(pct == NULL){
			perror("allocate city structure failed");
			return -1;
		}

		bzero(pct, sizeof(CT_T));
		sscanf(strline, "%s %s", pct->ct_id, pct->ct_nm);

		if(pct_head==NULL){
			pct_head = pct;
		}else{
			ct_index = pct_head;
			CT_T* ct_last;
			while(NULL!=ct_index){
				ct_last = ct_index;
				ct_index = ct_index->next;	
			}
			ct_last->next = pct;
		}
	}

	/*list all the city and its' id number*/
	ct_index = pct_head;
	while(ct_index!=NULL){
		printf("[CITY]%s=%s\n", ct_index->ct_id, ct_index->ct_nm);
		ct_index = ct_index->next;
	}
	/*free the getline allocate strline memory*/
	if(strline){
		free(strline);
	}
	printf("loading city information from %s success\n", ct_fnm);
	return 0;
}

void city_release(void){
	CT_T* index = pct_head;
	while(index){
		pct_head = index->next;
		free(index);
		index = pct_head;
	}
}

#undef __CHS_CT_C__
