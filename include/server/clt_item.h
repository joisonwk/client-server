#ifndef __CLT_ITEM_H__
#define __CLT_ITEM_H__

#include <stdio.h>
#include <server/server_core.h>
#include <server/conn_tcp.h>
#include <server/conn_udp.h>
enum eclt_type{
	ECT_UNKNOW,
	ECT_TCP,
	ECT_UDP,
};

typedef struct clt_item{
	enum eclt_type ci_type; //client type
	int ci_cfd;	//client socket fd
	struct timeval ci_last_time;
	struct timeval ci_timeval;	//ci_timeval=0 never timeout
	#define MAX_TIMEVAL	3600 	//1 hour 
	char ci_rbuf[MAX_RECV_LEN];
	size_t ci_rlen;
	char ci_wbuf[MAX_RECV_LEN];
	size_t ci_wlen;
	struct list_head ci_list;
}CLT_T;

/*
* test timeout
*/
static bool clt_is_timeout(CLT_T* pclt);
/*
* refresh the client last time
*/
extern void clt_fresh(CLT_T* pclt);

static void clt_release(struct list_head& head);

extern CLT_T* clt_malloc(void);

extern void clt_init(CLT_T* pclt);
extern int clt_add(struct list_head* head,CLT_T* pclt);
/*release the timeout client connect*/
extern void clt_flush(void);
extern void* clt_recv_data(void* pdata);

#endif//__CLT_ITEM_H__
