#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>
#include <ctype.h>
#include <stdint.h>

#ifdef INSTRUMENT_MAIN
void instrumentstats(char *prefixstring) ;

int main(int argc, char **argv) {

	instrumentstats("Test Main for instrumentation");

}

#endif


void instrumentstats(char *prefixstring) {
    FILE *fp;
    char buffer[5020];


	// Status for memory
    fp = fopen("/proc/self/status", "r");
    while (fgets(buffer, 5000, fp)) {
		if(strncmp(buffer,"Vm",(size_t)2) == 0) {
        	fputs(buffer, stdout);
		}
		else {
		}
    }
    printf("\n");
    fclose(fp);


	// status for cpu time, system and user

	struct rusage ruse ;



	int ret = getrusage(RUSAGE_SELF,&ruse);
	if(ret == -1) {
		perror("rusage()");
		exit(-1);
	}

	double ssec1, usec1;
    ssec1 = (double)(ruse.ru_stime.tv_sec * 1000000 + ruse.ru_stime.tv_usec);
    ssec1= ssec1/1000000;

    usec1 = (double)(ruse.ru_utime.tv_sec * 1000000 + ruse.ru_utime.tv_usec);
    usec1=usec1/1000000;


    char rw_buf[200];
    char seqran_buf[200];


    printf("%s , Total:%6.3lf,User:%6.3lf,System:%6.3lf\n", prefixstring, usec1+ssec1, usec1,ssec1);




    return;
}

