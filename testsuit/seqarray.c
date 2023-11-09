#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "instruments.h"
#define  MAXPAGES  2048

#define  JUMBOSIZE 4096*MAXPAGES

unsigned int array1[JUMBOSIZE];
unsigned int array2[JUMBOSIZE] ;

#define  MINCHUNKSIZE 5

int N = MINCHUNKSIZE ; 

// more for the pattern
unsigned int NUMBER1 = 29 ;
unsigned int NUMBER2 = 21;
unsigned int NUMBER3 = 10567 ;

int main(int argc, char **argv) {
	int headerflag ;

	if ( argc < 2 ) {
		fprintf(stderr,"Usage bigarray <chunks min %d, max 1000> -h\n",MINCHUNKSIZE);
		exit(1);
	}

	int pages = MAXPAGES ;
	int chunks = N  ; 
	int ret = 0 ;

	for (int i = 0 ; i < argc ; i ++) {
		printf("argv[%d] is %s\n",i, argv[i]);
	}

	if(argc == 2 ) {
		headerflag = 0 ; 
	}
	else {
		headerflag = 1 ;
	}
	
	ret = sscanf(argv[1],"%d",&chunks);
	if( ret != 1)  {
		fprintf(stderr, "chunks should be integer\n");
		fprintf(stderr,"Usage bigarray <pages>  <chunks>\n");
		exit(1);
	}

	if ( chunks < MINCHUNKSIZE || chunks > 1000 ) {
		fprintf(stderr,"chunk size reset to %d\n",MINCHUNKSIZE);
		N = MINCHUNKSIZE ;
	}

	N = chunks ;
	printf("pages %d chunks %d\n", pages, N);





	FILE *fp ; 
	char buffer[4000] ; 



// Write some memory
				printf("writing first pattern\n");


				for ( unsigned int i=0 ; i < JUMBOSIZE ; i ++) {
					array1[i] = i ;
					array2[i] = i*i ;
				}
				for (unsigned  int i=0 ; i < JUMBOSIZE ; i ++) {

					if( (array1[i] == i ) & (array2[i] == i*i)){
						continue;
					} else {
						printf("Memory error at location %d\n",i);
					}
				}

				char tmpBuf[200] ;
				sprintf(tmpBuf,"ProgramName,Pages,N");
				initheaders(tmpBuf);
				sprintf(tmpBuf,"Sequential Array,%d,%d",MAXPAGES,N);
				instrumentstats(tmpBuf,headerflag);
				exit(0);

}
