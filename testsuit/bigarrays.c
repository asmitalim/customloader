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
	/*
	int ret = sscanf(argv[1],"%d",&pages);
	if( ret != 1 ) {
		fprintf(stderr, "pages should be integer\n");
		fprintf(stderr,"Usage bigarray <pages>  <chunks>\n");
		exit(1);
	}
	*/
	
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
				unsigned int basenumber ; 
				basenumber = 0 ;

				//printf("i from %d to %d\n",basenumber, basenumber + JUMBOSIZE/N);

				for ( unsigned int i=basenumber ; i < (basenumber+JUMBOSIZE/N) ; i ++) {
					array1[i-basenumber] = (i-basenumber) % NUMBER1 ; 
					//if( (i+1)%100 == 0 ) printf(".");
					//if( (i+1)%1000 == 0 ) printf("\n");
				}
				//printf("done\n");

				basenumber = 3*JUMBOSIZE/N ; 

				//printf("i from %d to %d\n",basenumber, basenumber + 2*JUMBOSIZE/N);
				printf("writing second pattern\n");
				for (unsigned  int i= basenumber; i < (basenumber+2*(JUMBOSIZE/N)); i ++) {
					array2[i-basenumber] = NUMBER2*(i-basenumber) + ((i-basenumber) % NUMBER3) ; 
					//if( (i+1)%100 == 0 ) printf(".");
					//if( (i+1)%1000 == 0 ) printf("\n");
				}
				//printf("done\n\n\n");


				//printf("checking first pattern\n");
				basenumber = 0 ;
				//printf("i from %d to %d\n",basenumber, basenumber + JUMBOSIZE/N);
				for (unsigned  int i=basenumber ; i < (basenumber+JUMBOSIZE/N) ; i ++) {

					if( array1[i-basenumber] == ((i-basenumber)% NUMBER1) )  {

						//if( (i+1)%100 == 0 ) printf(".");
						//if( (i+1)%1000 == 0 ) printf("\n");
						continue;
					} else {
						printf("Memory error at location %d\n",i);
						exit(1);
					}
				}
				//printf("done\n");
				//printf("checking second pattern\n");
				basenumber = 3*JUMBOSIZE/N ; 
				//printf("i from %d to %d\n",basenumber, basenumber + 2*JUMBOSIZE/N);
				for (unsigned  int i= basenumber; i < (basenumber+2*(JUMBOSIZE/N)); i ++) {
					if( array2[i-basenumber] == 
						((NUMBER2*(i-basenumber) + ((i-basenumber) % NUMBER3)))) {
						//if( (i+1)%100 == 0 ) printf(".");
						//if( (i+1)%1000 == 0 ) printf("\n");
						continue;
					}
					else {
						printf("error:Memory error at location %d\n",i);
						exit(1);
					}
				//printf("done\n\n");
				}

				/*
				fp =  fopen("/proc/self/maps", "r");
				if( fp == (FILE *)NULL ) {
					perror("fopen():");
					fprintf(stderr,"can not open the status\n");
					exit(1);
				}


				printf("residue status of the file\n");
				while(fgets(buffer,4950,fp)) {
					fputs(buffer,stdout);
				}
				fputs("\n",fp);

				fclose(fp);
				*/
				char tmpBuf[200] ;
				sprintf(tmpBuf,"ProgramName,Pages,N");
				initheaders(tmpBuf);
				sprintf(tmpBuf,"BigArrays,%d,%d",MAXPAGES,N);
				instrumentstats(tmpBuf,headerflag);
				exit(0);

}
