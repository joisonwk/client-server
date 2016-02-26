/*src/process/chs_ctr.c*/
#define __CHS_CTR_C__
#include <process/chs_ctr.h>
#include <stdio.h>
#include <string.h>

static CTR_T* ctr_head = NULL;
/*ctr_id to country name*/
int id_to_country(const char* ctr_id, char* ctr_nm, int ctr_len){
	int cnlen = 0;
	CTR_T* ctr_tmp = ctr_head;
	while(NULL != ctr_tmp){
		if(!strncmp(ctr_tmp->ctr_id,ctr_id,2)){
			cnlen += snprintf(ctr_nm, ctr_len, ctr_tmp->ctr_nm);	
			break;
		}
		ctr_tmp = ctr_tmp->next;
	}
	if(cnlen == 0)
		cnlen += snprintf(ctr_nm, ctr_len, "UNKNOW_COUNTRY");	
	return cnlen;
}

int country_init(const char* ctr_fnm){
	FILE* fp = NULL;
	char* strline = NULL;
	int llen = 0;
	char ctr_nm[128];
	CTR_T* ctr_index = NULL; 
	if(ctr_fnm == NULL){
		printf("country file name is empty\n");
		return -1;
	}

	/*open country file*/
	fp = fopen(ctr_fnm, "r");
	if(fp == NULL){
		char errinfo[128];
		sprintf(errinfo, "open [%s] failed", ctr_fnm);
		perror(errinfo);	
		return -1;
	}

	/*loading country info to country list*/
	int getlen = 0;
	while(-1 != getline(&strline, &llen, fp)){
		CTR_T* pctr = (CTR_T*)malloc(sizeof(CTR_T));
		if(pctr == NULL){
			perror("allocate country structure failed");
			return -1;
		}
		bzero(pctr, sizeof(CTR_T));

		char id_buf[128];
		sscanf(strline, "%s %s", pctr->ctr_id, pctr->ctr_nm);

		if(ctr_head==NULL){
			ctr_head = pctr;
		}else{
			ctr_index = ctr_head;
			CTR_T *ctr_last;
			while(ctr_index){
				ctr_last = ctr_index;
				ctr_index= ctr_index->next;
			}
			ctr_last->next = pctr;
		}
	}

	/*free the getline allocate strline memory*/
	if(strline){
		free(strline);
	}

	printf("loading country information from %s success\n", ctr_fnm);
	ctr_index = ctr_head;
	while(ctr_index){
		printf("[CTR]%s = %s\n", ctr_index->ctr_id, ctr_index->ctr_nm);
		ctr_index = ctr_index->next;
	}
	return 0;
}

void country_release(void){
	CTR_T* index = ctr_head;
	while(index){
		ctr_head = index->next;
		free(index);
		index = ctr_head;
	}
}

#undef __CHS_CTR_C__
