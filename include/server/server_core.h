/*include/server/server_core.h*/
#ifndef __SERVER_CORE_H__
#define __SERVER_CORE_H__
#define SERVER_CONF_FILE	"server.conf"
#include <sys/select.h>
#include <util/c_list.h>
#include <server/clt_item.h>
#include <server/process_server.h>
#include <server/conn_tcp.h>

struct server_data{
	sem_t sd_sem;	//server semephore
	/*connect configure struct*/
	unsigned int sd_connflag;
	struct tcp_data sd_tcpconf;
	struct udp_data sd_udpconf;

	struct list_head sd_clt_head;
	//CLT_T* sd_clts;	
	struct list_head sd_pd_head;
	//PD_T* sd_pd;

	pthread_t sd_tcp_pid;	//tcp receive thread id 
	pthread_t sd_clt_recv_pid;
	pthread_t sd_prcs_pid;	//process thread id

	unsigned int ;	//current conn number
	fd_set clts_fds;	//client read fd_set
};

static void flush_sys(void);
extern int server_pd_add(PD_T* pd);

#endif //__SERVER_CORE_H__
