/*include/common.h*/
#ifndef __CS_COMMON_H__
#define __CS_COMMON_H__

/*tcp default define*/
#define TCP_PORT_DEFAULT 2323
#define TCP_MAX_CONN_DEFAULT 16
#define TCP_DOMAIN_DEFAULT AF_INET

/*udp default define*/
#define UDP_SERVER_PORT 8888

/*protocol define*/
#define MAX_RECV_LEN 4096
#define MAX_FRAME_LEN 1024
#define FRAME_HEAD	0x55
#define MAX_CMD_LEN 1021 //(MAX_FRAME_LEN-HEAD(1byte)-FRAME_LEN(2byte))	

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

#endif //__CS_COMMON_H__
