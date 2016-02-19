#ifndef __CONN_TCP_H__
#define __CONN_TCP_H__
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
/*tcp server configure struct*/
struct tcp_data{
	pthread_t tcp_recv_pid;
	pthread_t tcp_send_pid;
	unsigned short tcp_port;
	int tcp_domain;	// domain indicate
	size_t tcp_max_lstn;	//max listen
	int tcp_sfd;
	#define DEFAULT_LSTNUM	16
};

extern void* tcp_thread(void* psd);
/*tcp client recieve data thread func*/
static void* tcp_clt_data_recv(void* pdata);
/*tcp client send data thread func*/
static void* tcp_clt_data_send(void* pdata);

#endif //__CONN_TCP_H__
