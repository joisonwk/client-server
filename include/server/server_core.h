/*include/server/server_core.h*/
#ifndef __SERVER_CORE_H__
#define __SERVER_CORE_H__
#define SERVER_CONF_FILE	"server.conf"
#include <sys/select.h>
#include <server/clt_item.h>
#include <server/process_server.h>
#include <server/conn_tcp.h>

struct server_data{
	int sd_fd; //server socket fd
	CLT_T* sd_clts;	
	PD_T* sd_pd;

	unsigned short sd_tcp_port;
	unsigned short sd_udp_port;

	pthread_t sd_recvpid;	//receive thread id 
	pthread_t sd_prcspid;	//process thread id

	unsigned int td_max_conns;
	unsigned int td_cur_clt_num;	//current conn number
	time_t td_clt_tm_threshold;	//client connect timeout threshold
	fd_set clts_fds;	//client read fd_set
};

extern void flush_sys(void);
extern int server_clt_add(CLT_T* clt);
extern int server_pd_add(PD_T* pd);

#endif //__SERVER_CORE_H__
