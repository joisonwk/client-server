/*include/server/clt_item.h*/
#ifndef __CLT_ITEM_H__
#define __CLT_ITEM_H__

#include <stdio.h>
#include <server/server_core.h>

enum eclt_type{
	ECLT_UNKNOW,
	ECLT_TCP,
	ECLT_UDP,
};

typedef struct clt_item{
	enum eclt_type ci_type; //client type
	int ci_cfd;	//client socket fd
	struct timeval ci_conn_time;
	char* ci_pbuf[MAX_RECV_LEN];
	ssize_t ci_recvlen;
	struct clt_prew* prew;
	struct clt_item* next;
}CLT_T;

static void clt_add(struct server_data* pdata, CLT_T* pclt);
static void clt_release(CLT_T* pclt);
/*release the timeout client connect*/
extern void clt_flush(struct server* pserver);
/*destroy the client connect*/
extern void clt_items_destroy(CLT_T* clts);
/*client receive data*/
extern void* tcp_clt_recv_thread(void* pdata);

#endif//__CLT_ITEM_H__
