#ifndef __CLT_ITEM_H__
#define __CLT_ITEM_H__
#include <stdio.h>
#include <stddef.h>
#include <semaphore.h>
#include <util/c_list.h>
#include <conn/conn_tcp.h>
#include <conn/conn_udp.h>
#include <conn/conn_common.h>

enum eclt_type{
	ECT_UNKNOW,
	ECT_TCP,
	ECT_UDP,
};

typedef struct clt_item{
	sem_t ci_sem;
	enum eclt_type ci_type; //client type
	int ci_cfd;	//client socket fd
	struct timeval ci_last_time;
	struct timeval ci_timeval;	//ci_timeval=0 never timeout
	#define MAX_TIMEVAL	3600 	//1 hour 
	//unsigned int ci_step_flag;	//recording deal steps
	char ci_rbuf[MAX_RECV_LEN];
	size_t ci_rlen;
	char ci_wbuf[MAX_SEND_LEN];
	size_t ci_wlen;
	struct list_head ci_list;
}CLT_T;

/*
* test timeout, timeout return 1, else return 0
*/
static int clt_is_timeout(CLT_T* pclt);

extern void clt_release(CLT_T* pclt);
/*
* refresh the client last time
*/
extern void clt_fresh(CLT_T* pclt);

extern void clt_exit(void);

extern CLT_T* clt_malloc(void);

/*initiate the client, sem_init client semaphore and list struct*/
extern void clt_init(CLT_T* pclt);

/*insert client success return 0, else return -1*/
extern int clt_add(CLT_T* pclt);

/*release the timeout client connect*/
extern void clt_flush(void);

#endif//__CLT_ITEM_H__
