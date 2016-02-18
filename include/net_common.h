/*include/net_common.h*/
#ifndef __NET_COMMON_H__
#define __NET_COMMON_H__

/*tcp default define*/
#define TCP_PORT_DEFAULT 2323
#define TCP_MAX_CONN_DEFAULT 16
#define TCP_DOMAIN_DEFAULT AF_INET

/*udp default define*/
#define UDP_SERVER_PORT 8888

/*protocol define*/
#define MAX_RECV_LEN 4096
#define MAX_SND_LEN 4096

/*client timeout limit*/
#define DEFAULT_THRESHOLD	20 	
#define MAX_THRESHOLD		600	//10 mins

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({ \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#endif //__NET_COMMON_H__
