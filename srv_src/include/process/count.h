#ifndef __COUNT_H__
#define __COUNT_H__
#ifdef __COUNT_C__
#define COUNT_EXTERN 
#else
#define COUNT_EXTERN extern
#endif //__COUNT_C__
#include <semaphore.h>

struct count_data{
	sem_t cd_sem;
	struct timeval cd_stm;	//start count time	
	//struct timeval cd_etm;	//end count time
	unsigned int cd_ovs;	// over longlong length counts
	unsigned long long int cd_tcp_bytes;	//recieve tcp bytes
	//long long cd_udp_bytes; //recieve udp bytes
};

static int count_init(void* pdata);
static void count_deal(CLT_T* pclt);
static void count_exit(void);

COUNT_EXTERN void count_show(void);


#endif //__COUNT_H__
