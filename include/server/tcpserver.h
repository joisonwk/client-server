/*include/server/tcpserver.h*/
#ifndef __CONN_TCP_H__
#define __CONN_TCP_H__
#include <sys/socket.h>
#include <conn_common.h>
#include <server/server_core.h>
#include <sys/time.h>

extern int tcp_init(struct server_data* pdev);

#endif //__CONN_TCP_H__
