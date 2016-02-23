/*include/server/server_core.h*/
#ifndef __SERVER_CORE_H__
#define __SERVER_CORE_H__
#define SERVER_CONF_FILE	"server.conf"
#include <sys/select.h>
#include <semaphore.h>
#include <sys/types.h>
#include <util/c_list.h>
#include <conn/conn_tcp.h>
#include <conn/conn_udp.h>

struct server_data{
	sem_t sd_sem;	//server semephore

	/*connect configure struct*/
	struct tcp_data sd_tcpconf;	//
	struct udp_data sd_udpconf;

	struct list_head sd_clt_head;	//client list head
	struct list_head sd_pm_head;	//process method list head

	/*connection thread id*/
	pthread_t sd_tcp_pid;	//tcp receive thread id 
	/*process thread id*/
	pthread_t sd_prcs_pid;	//process thread id

	fd_set clts_fds;	//client read fd_set
};

extern void flush_sys(void);
extern int server_init(void);

extern struct server_data* get_server(void);
extern void post_server(void);


#endif //__SERVER_CORE_H__
