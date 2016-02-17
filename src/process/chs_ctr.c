/*src/process/chs_ctr.c*/
#define __CHS_CTR_C__
#include <process/chs_ctr.h>

static CTR_T* ctr_head;
/*ctr_id to country name*/
int id_to_country(PROVINCE_ID ctr_id, char* ctr_nm, unsigned int ctr_len){
	int ctr_exist = 0;

	CTR_T ctr_tmp = ctr_head;
	while(ctr_tmp){
		if(ctr_tmp->ctr_id == ctr_id){
			snprintf(ctr_nm, ctr_len, ctr_tmp->ctr_nm);	
			ctr_exist = 1;
		}
		ctr_tmp = ctr_tmp->next;
	}
	if(!ctr_exist)
		snprintf(ctr_nm, ctr_len, "UNKNOW_COUNTRY");	
}

int country_init(const char* ctr_fnm){
	FILE* fp = NULL;
	char* strline = NULL;
	char ctr_nm[128];
	if(ctr_fnm == NULL){
		printf("country file name is empty\n");
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
	while(getline(&strline, NULL, fp)){
		CTR_T* pctr = malloc(sizeof(CTR_T));
		if(pctr == NULL){
			perror("allocate country structure failed");
			return -1;
		}

		char id_buf[128];
		sscanf(strline, "%s=%s", id_buf, pctr->ctr_nm);
		pctr->ctr_id = atoi(id_buf);			
	}
	/*free the getline allocate strline memory*/
	if(strline){
		free(strline);
	}
}

void country_release(void){
	CTR_T* index = NULL ;
	while(ctr_head){
		index = ctr_head;
		free(index);
		ctr_head = ctr_head->next;
	}
}

#undef __CHS_CTR_C__
