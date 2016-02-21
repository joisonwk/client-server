#ifndef __CONN_TCP_H__
#define __CONN_TCP_H__
#include <stddef.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
/*tcp server configure struct*/
struct tcp_data{
	pthread_t tcp_recv_pid;	//tcp client data recieve thread id
	pthread_t tcp_send_pid;	//tcp client data send thread id
	unsigned short tcp_port;	//tcp server port
	int tcp_domain;		// tcp server domain indicate
	size_t tcp_max_lstn;	//max server listen number
	#define DEFAULT_LSTNUM	16	//if the max listen number is not indicated, using default listen number 
	int tcp_sfd;		//tcp server socket file descriptor
};

/*tcp client recieve data thread func*/
static void* tcp_clt_data_recv(void* pdata);
/*tcp client send data thread func*/
static void* tcp_clt_data_send(void* pdata);

/*create tcp server thread*/
extern void* tcp_thread(void* psd);

#endif //__CONN_TCP_H__
