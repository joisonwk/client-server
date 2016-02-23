#ifndef __CONN_TCP_H__
#define __CONN_TCP_H__
#include <conn_item.h>
extern int id_to_addr(const char* id_buf, char* addr_buf, int* paddrlen);

extern int tcp_init(struct conn_item* pdata);
#endif	//__CONN_TCP_H__
