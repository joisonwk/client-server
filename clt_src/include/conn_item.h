#ifndef __CONN_ITEM_H__
#define __CONN_ITEM_H__

#include <sys/socket.h>
/*tcp default define*/
#define TCP_PORT 2323
#define TCP_CLTS 16

/*udp default define*/
#define UDP_PORT 8888

/*protocol define*/
#define MAX_RECV_LEN 4096
#define MAX_SEND_LEN 4096

enum conn_type{
	CT_UNKOWN,
	CT_TCP,
};

enum conn_stat{
	CT_UNCONN,	//unconnected
	CT_CONNED,	//connnected
};

struct conn_item{
	enum conn_type ci_ct;	//connect type
	enum conn_stat ci_cs;	//connect status
	int ci_sockfd;
	struct sockaddr_in ci_saddr;
};
#endif
