/*process/statistics.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <server/clt_item.h>
#define FILE_NAME "recieve.data"

static int fd = -1;
static void __save_to_file_init(CLT_T* pclt);

void (*save_to_file)(CLT_T* pclt) = 
	__save_to_file_init;

static void __save_to_file_null(CLT_T* pclt){
	printf("%s\n", __func__);
	/*empty*/
}
static void __save_to_file_kenerl(CLT_T* pclt){
	char buf[4096];
	int len = 0;
	len = pclt->ci_rlen<4096? pclt->ci_rlen:4096;
	memcpy(buf, pclt->ci_rbuf, pclt->ci_rlen);
	write(fd,buf,len);
	printf("%s[%s]\n", __func__, buf);
	system("sync");
}

void __save_to_file_init(CLT_T* pclt){
	printf("%s\n", __func__);
	if(save_to_file==__save_to_file_init){
		fd = open(FILE_NAME, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	}
	save_to_file = __save_to_file_kenerl;

	if(fd == -1){
		save_to_file = __save_to_file_null;
		perror("open reieve.data failed");
	}
	save_to_file(pclt);
	
}
