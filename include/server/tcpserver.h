/*include/server/tcpserver.h*/
#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__
/*define extern */
#ifndef __TCPSERVER_C__
#define  TCPSERVER_EXTERN extern
#else
#define  TCPSERVER_EXTERN
#endif //__TCPSERVER_C__

#include <sys/select.h>
#include <sys/socket.h>
#include <net_common.h>
#include <process/process_core.h>
#include <sys/time.h>


typedef struct clt_info {
	//sem_t ci_rcv_sem;	//when recieve success post sem, and the parse thread get sem continue
	sem_t ci_sem;	//when process success post sem, send thread get sem continue
	int ci_cfd;	//client socket fd
	pthread_t ci_pid;	//client transfer thread
	struct timeval ci_conn_time;
	//struct sockaddr_in ci_addr;	//client addr
	//int ci_sinlen;	//client addr size

	char ci_recvbuf[MAX_RECV_LEN];
	int ci_recvlen;
	char ci_sndbuf[MAX_SND_LEN];
	int ci_sndlen;
	
	PD_T* ci_pdlist;

	struct clt_info* next;
} CLT_INFO_T;

struct ts_data{
	sem_t td_lock;
	SERVER_TYPE_T td_method;
	int td_sfd; //server socket fd
	struct ts_dev td_dev;
	pthread_t td_conn_pid;	
	pthread_t td_recv_pid;
	pthread_t td_process_pid;

	CLT_INFO_T* td_clts_head;	//clients conn info
	unsigned int td_max_conns;
	unsigned int td_cur_clt_num;	//current conn number
	time_t td_clt_tm_threshold;	//client connect timeout threshold
	fd_set td_recv_fds;	//client read fd_set
	fd_set td_snd_fds;
};

struct ts_dev {
	unsigned short port;
	unsigned int max_conn;
	int domain;
	time_t tm_threshold;
};

TCPSERVER_EXTERN int tcp_server_init(void* pdev);
TCPSERVER_EXTERN void tcp_server_exit(void);

/*create the server socket*/
static int tcp_server_create(void* pdata);
static void tcp_server_lock_init(struct ts_data* pdata);
static int tcp_server_lock_get(struct ts_data* pdata);
static void tcp_server_lock_release(struct ts_data* pdata);

static void clt_init(CLT_INFO_T* pclt);
static void clt_release(CLT_INFO_T* pclt);
static int tcp_server_clt_add(struct ts_data* pdata, CLT_INFO_T* clt);
/*tcp connects stack fresh*/
static int tcp_server_connfresh(struct ts_data* pdata);


/*into receive moding*/
static void* tcp_server_conn(void* pdata);
/*client receive data*/
static void* tcp_server_recv(void* pdata);
/*process the client data*/
static void* tcp_server_process(void* pdata);

#endif //__TCPSERVER_H__
