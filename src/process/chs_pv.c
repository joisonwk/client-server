/*src/process/chs_pv.c*/
#define __CHS_PV_C__
#include <process/id_chs.h>
#include <process/chs_pv.h>

static PV_T* pv_head;
/*pv_id to province name*/
int id_to_province(unsigned int pv_id, char* pv_nm, unsigned int pv_len){
	int pv_exist = 0;

	PV_T pv_tmp = pv_head;
	while(pv_tmp){
		if(pv_tmp->pv_id == pv_id){
			snprintf(pv_nm, pv_len, pv_tmp->pv_nm);	
			pv_exist = 1;
		}
		pv_tmp = pv_tmp->next;
	}
	if(!pv_exist)
		snprintf(pv_nm, pv_len, "UNKNOW_PROVINCE");	
}

int province_init(const char* pv_fnm){
	FILE* fp = NULL;
	char* strline = NULL;
	char pv_nm[128];
	if(pv_fnm == NULL){
		printf("province file name is empty\n");
	}

	/*open province file*/
	fp = fopen(pv_fnm, "r");
	if(fp == NULL){
		char errinfo[128];
		sprintf(errinfo, "open [%s] failed", pv_fnm);
		perror(errinfo);	
		return -1;
	}

	/*loading province info to province list*/
	while(getline(&strline, NULL, fp)){
		PV_T* ppv = malloc(sizeof(PV_T));
		if(ppv == NULL){
			perror("allocate province structure failed");
			return -1;
		}

		char id_buf[128];
		sscanf(strline, "%s=%s", id_buf, ppv->pv_nm);
		ppv->pv_id = atoi(id_buf);			
	}
	/*free the getline allocate strline memory*/
	if(strline){
		free(strline);
	}
}

void province_release(void){
	PV_T* index = NULL ;
	while(pv_head){
		index = pv_head;
		free(index);
		pv_head = pv_head->next;
	}
}

#undef __CHS_PV_C__
