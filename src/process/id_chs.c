/*src/process/id_chs.c*/
#define __ID_CHS_C__

#include <string.h>
#include <ctype.h>


/*parse the id_num to id info, return the info length
* errno return -1
*/
int id_parse(const char* id_num, int id_len, char* retstr, int retlen){
	int ret, curlen;
	char cdigit = 0;
	int pv_id, ct_id, ctr_id;
	char nmbuf[128];
	struct id_info id_info;
	if(id_num == NULL){
		return -1;
	}

	ret = id_check(id_num)
	if(ret != 0){
		snprintf(retstr, retlen,"%s:this ID is illegal!\n", id_num);
	}

	pv_id = (id_num[0]-'0')* 10 + (id_num[1]-'0');	
	ct_id = (id_num[2]-'0')* 10 + (id_num[3]-'0');	
	ctr_id = (id_num[4]-'0')* 10 + (id_num[5]-'0');	
	id_to_province(pv_id, retstr, &curlen);
}



/*check the id_num ascll value, and the certifiation val*/
int id_check(const char* id_num, int id_len){
	if(id_len!=CHS_ID_LEN){
		return -1;
	}
		
	for(int i=0; i<17; i++){
		if(!isdigit(id_num[i])){
			return -1;
		}
	}

	if(!id_lastnum_chk_ok()){
		return -1;
	}
}

/*when the lastnum is right return true 1*/
int id_lastnum_chk_ok(const char* id_num){
	/*uncomplete:*/
	return 1;
}

/*id parse initiate*/
int id_parse_init(void){
	/*province info initiate*/	
	/*city info initiate*/
	/*country info initiate*/
}

/*id parse exit*/
void id_parse_exit(void){
	/*province release*/
	/*city release*/
	/*country release*/
}

#undef __ID_CHS_C__
