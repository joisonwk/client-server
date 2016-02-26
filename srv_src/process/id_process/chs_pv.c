/*src/process/chs_pv.c*/
#define __CHS_PV_C__
#include <stdio.h>
#include <string.h>
#include <process/id_chs.h>
#include <process/chs_pv.h>

static PV_T* pv_head = NULL;
/*pv_id to province name*/
int id_to_province(const char* pv_id, char* pv_nm, int pv_len){
	int pnlen= 0;
	PV_T* pv_tmp = pv_head;
	while(NULL != pv_tmp){
		if(!strncmp(pv_id, pv_tmp->pv_id,2)){
			pnlen += snprintf(pv_nm, pv_len, pv_tmp->pv_nm);	
			break;
		}
		pv_tmp = pv_tmp->next;
	}
	if(0 == pnlen)
		pnlen = snprintf(pv_nm, pv_len, "UNKNOW_PROVINCE");	
	return pnlen;
}

int province_init(const char* pv_fnm){
	FILE* fp = NULL;
	char* strline = NULL;
	PV_T* pv_index = NULL;
	int llen = 0;
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
	while(getline(&strline, &llen, fp)!=-1){
		//fseek(fp, llen, SEEK_CUR);
		PV_T* ppv = (PV_T*)malloc(sizeof(PV_T));
		if(ppv == NULL){
			perror("allocate province structure failed");
			return -1;
		}
		bzero(ppv, sizeof(PV_T));

		sscanf(strline, "%s %s", ppv->pv_id, ppv->pv_nm);
		if(pv_head == NULL){
			pv_head = ppv;
		}else{
			pv_index = pv_head;
			PV_T* pv_last = NULL;
			while(pv_index){
				pv_last = pv_index;
				pv_index = pv_index->next;
			}
			pv_last->next = ppv;
		}
	}
	/*free the getline allocate strline memory*/
	if(strline){
		free(strline);
	}
	
	pv_index = pv_head;
	while(pv_index!=NULL){
		printf("[PV] %s=%s\n", pv_index->pv_id, pv_index->pv_nm );
		pv_index = pv_index->next;
	}
	return 0;
}

void province_release(void){
	PV_T* index = pv_head;
	while(index){
		pv_head = index->next;
		free(index);
		index = pv_head;
	}
}

#undef __CHS_PV_C__
