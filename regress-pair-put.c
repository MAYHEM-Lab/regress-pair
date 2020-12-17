#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "woofc.h"
#include "regress-pair.h"

#define ARGS "W:s:LT:"
char *Usage = "senspot-put -W woof_name for put\n\
\t-s series type ('m' measured or 'p' predicted)\n\
\t-L use same namespace for source and target\n";

char Wname[4096];
char NameSpace[4096];
char Namelog_dir[4096];
char putbuf1[4096];
char putbuf2[4096];

#define MAX_RETRIES 20

#define CONVERT_TIME(ts,psec,usec) {\
	psec = htonl((unsigned int)ts);\
	usec = htonl((unsigned int)((ts - (double)ntohl(psec))*1000000.0));\
}

int main(int argc, char **argv)
{
	int c;
	int i;
	int err;
	int uselocal;
	unsigned char input_buf[4096];
	char *str;
	REGRESSVAL rv;
	char wname[4096];
	char type;
	char series_type;
	unsigned long seq_no;
	struct timeval tm;
	double value;
	double ts;
	char *next_str;

	memset(wname,0,sizeof(wname));
	uselocal = 0;
	series_type = 0;

	while((c = getopt(argc,argv,ARGS)) != EOF) {
		switch(c) {
			case 'W':
				strncpy(wname,optarg,sizeof(wname));
				break;
			case 's':
				series_type = optarg[0];
				break;
			case 'L':
				uselocal = 1;
				break;
			default:
				fprintf(stderr,
				"unrecognized command %c\n",(char)c);
				fprintf(stderr,"%s",Usage);
				exit(1);
		}
	}

	if(wname[0] == 0) {
		fprintf(stderr,"must specify filename for target object\n");
		fprintf(stderr,"%s",Usage);
		fflush(stderr);
		exit(1);
	}

	if((series_type != 'm') && (series_type != 'p')) {
		fprintf(stderr,
			"must specify either m or p for series type\n");
		fprintf(stderr,"%s",Usage);
		exit(1);
	}

	if(Namelog_dir[0] != 0) {
		sprintf(putbuf2,"WOOF_NAMELOG_DIR=%s",Namelog_dir);
		putenv(putbuf2);
	}

	if(uselocal == 1) {
		WooFInit();
	}

	memset(input_buf,0,sizeof(input_buf));
	str = fgets(input_buf,sizeof(input_buf),stdin);

	if(str == NULL) {
		exit(0);
	}
	memset(&rv,0,sizeof(rv));

	strncpy(rv.woof_name,wname,sizeof(rv.woof_name));
	rv.series_type = series_type;

	/*
	 * ts needs to be a number
	 * value needs to be a number
	 */
	ts = strtod(input_buf,&next_str);
	value = strtod(next_str,NULL);
	
	rv.value.d = value;
	rv.type = 'd';

	CONVERT_TIME(ts,rv.tv_sec,rv.tv_usec);

	WooFLocalIP(rv.ip_addr, sizeof(rv.ip_addr));

	seq_no = WooFPut(wname,"RegressPairReqHandler",(void *)&rv);

	if(WooFInvalid(seq_no)) {
		fprintf(stderr,"request-pair-put failed for %s with handler %s type %c and cargo %s\n",
			wname,
			"RegressPairReqHandler",
			type,
			input_buf);
		fflush(stderr);
		exit(1);
	}

	fprintf(stdout, "%lu", seq_no);
	fflush(stdout);

	exit(0);
}

	

	
	
