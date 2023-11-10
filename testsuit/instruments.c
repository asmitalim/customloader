#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>
#include <ctype.h>
#include <stdint.h>

#include "instruments.h"


#ifdef INSTRUMENT_MAIN

int main(int argc, char **argv) {

	initheaders("Title");
	instrumentstats("Test Main for instrumentation",1);

}

#endif


char *statusheader = "vmpeak,vmsize,vmhwm,vmdata,vmstack,vmexec,vmlibd,vmswap,totaltime,usertime,systemtime\n";
char *statusformat = "%d,%d,%d,%d,%d,%d,%d,%d,%6.3lf,%6.3lf,%6.3lf\n" ;

static char headerbuffer[1000] ;
static char resultbuffer[1000] ;


void initheaders(char *prefixheader) {
	
	sprintf(headerbuffer,"%s,%s",prefixheader,statusheader);
}

void instrumentstats(char *prefixstring, int headerflag) {
    FILE *fp;
    char buffer[5020];
	struct rusage ruse ;
	int ret ; 
	char junk0[100];
	char junk1[100];

	int vmpeak  = -1;
	int vmsize = -1 ;
	int vmhwm  = -1;
	int vmdata  = -1 ;
	int vmstk = -1;
	int vmexe  = -1;
	int vmlib = -1;
	int vmswap = -1;


	// Status for memory
    fp = fopen("/proc/self/status", "r");
    while (fgets(buffer, 5000, fp)) {
		if(strncmp(buffer,"Vm",(size_t)2) == 0){ 
			if(strncmp(buffer,"VmPeak",(size_t)6) == 0 ) sscanf(buffer,"%s %d %s",junk0,&vmpeak, junk1);
			if(strncmp(buffer,"VmSize",(size_t)6) == 0 ) sscanf(buffer,"%s %d %s",junk0,&vmsize, junk1);
			if(strncmp(buffer,"VmHWM",(size_t)5) == 0 ) sscanf(buffer,"%s %d %s",junk0,&vmhwm, junk1);
			if(strncmp(buffer,"VmData",(size_t)6) == 0 ) sscanf(buffer,"%s %d %s",junk0,&vmdata, junk1);
			if(strncmp(buffer,"VmStk",(size_t)5) == 0 ) sscanf(buffer,"%s %d %s",junk0,&vmstk, junk1);
			if(strncmp(buffer,"VmExe",(size_t)5) == 0 ) sscanf(buffer,"%s %d %s",junk0,&vmexe, junk1);
			if(strncmp(buffer,"VmLib",(size_t)5) == 0 ) sscanf(buffer,"%s %d %s",junk0,&vmlib, junk1);
			if(strncmp(buffer,"VmSwap",(size_t)6) == 0 ) sscanf(buffer,"%s %d %s",junk0,&vmswap, junk1);
		}
       	//fputs(buffer, stdout);
	}
    printf("\n");
    fclose(fp);


	// status for cpu time, system and user




	ret = getrusage(RUSAGE_SELF,&ruse);
	if(ret == -1) {
		perror("rusage()");
		exit(-1);
	}

	double ssec1, usec1;
    ssec1 = (double)(ruse.ru_stime.tv_sec * 1000000 + ruse.ru_stime.tv_usec);
    ssec1= ssec1/1000000;

    usec1 = (double)(ruse.ru_utime.tv_sec * 1000000 + ruse.ru_utime.tv_usec);
    usec1=usec1/1000000;


	FILE *fpresult = fopen("./results.log","a+") ;


	if(headerflag) { 
		printf("%s",headerbuffer);
		fputs(headerbuffer,fpresult);

    	sprintf(buffer,"%s,%s",prefixstring,statusformat);
		sprintf(resultbuffer,buffer, 
			vmpeak,vmsize,vmhwm,vmdata,vmstk,vmexe,vmlib,vmswap,
			usec1+ssec1, usec1,ssec1);
		fputs(resultbuffer,fpresult);
		printf("%s",resultbuffer);
	}
	else {
    	sprintf(buffer,"%s,%s",prefixstring,statusformat);
		sprintf(resultbuffer,buffer, 
			vmpeak,vmsize,vmhwm,vmdata,vmstk,vmexe,vmlib,vmswap,
			 usec1+ssec1, usec1,ssec1);
		fputs(resultbuffer,fpresult);
		printf("%s",resultbuffer);
	}

	fclose(fpresult);
    return;
}

