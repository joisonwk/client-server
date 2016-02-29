/*srv_src/process/err_process/data_err.c*/
#define __DATA_ERR_C__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <server/clt_item.h>
#include <process/data_err.h>
#include <process/process_core.h>

struct data_err_data* pded = NULL;

int data_err_init(void* pdata){
	if(pded == NULL){
		pded = malloc(sizeof(struct data_err_data));
		if(pded == NULL){
			return -1;
		}
		bzero(pded, sizeof(struct data_err_data));
	}
}

void data_err_deal(CLT_T* pclt){
	
}

void data_err_exit(void){
	if(pded){
		free(pded);
		pded = NULL;
	}
}

#undef __DATA_ERR_C__
