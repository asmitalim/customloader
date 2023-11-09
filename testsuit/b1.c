#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#define  JUMBOSIZE 4096*10

unsigned int array1[JUMBOSIZE];
unsigned char array2[JUMBOSIZE] ;

int N = 5000 ; 

unsigned int NUMBER1 = 29 ;
unsigned int NUMBER2 = 21;
unsigned int NUMBER3 = 10567 ;

int main(int argc, char **argv) {
	FILE *fp ; 
	char buffer[4000] ; 

	assert( N > 6);


// Write some memory
				printf("writing first pattern\n");
				unsigned int basenumber ; 
				basenumber = 0 ;

				printf("i from %d to %d\n",basenumber, basenumber + JUMBOSIZE/N);

				for ( unsigned int i=basenumber ; i < (basenumber+JUMBOSIZE/N) ; i ++) {
					array1[i-basenumber] = (i-basenumber) % NUMBER1 ; 
					if( (i-basenumber) < (NUMBER1 +10)) {
						printf("A[%d] = %d\n",i-basenumber,array1[i-basenumber]) ;
					}
					if( (i+1)%100 == 0 ) printf(".");
					if( (i+1)%1000 == 0 ) printf("\n");
				}
				printf("done\n");

				basenumber = 3*JUMBOSIZE/N ; 

				printf("i from %d to %d\n",basenumber, basenumber + 2*JUMBOSIZE/N);
				printf("writing second pattern\n");
				for (unsigned  int i= basenumber; i < (basenumber+2*(JUMBOSIZE/N)); i ++) {
					array2[i-basenumber] = NUMBER2*(i-basenumber) + ((i-basenumber) % NUMBER3) ; 
					if( (i-basenumber) < (NUMBER1 +10)) {
						printf("B[%d] = %d\n",i-basenumber,array2[i-basenumber] );
					}
					if( (i+1)%100 == 0 ) printf(".");
					if( (i+1)%1000 == 0 ) printf("\n");
				}
				printf("done\n\n\n");


				printf("checking first pattern\n");
				basenumber = 0 ;
				printf("i from %d to %d\n",basenumber, basenumber + JUMBOSIZE/N);
				for (unsigned  int i=basenumber ; i < (basenumber+JUMBOSIZE/N) ; i ++) {

					if( array1[i-basenumber] == ((i-basenumber)% NUMBER1) )  {

						if( (i-basenumber) < (NUMBER1 +10)) {
								printf("A[%d] = %d\n",i-basenumber,array1[i-basenumber] );
							}
						if( (i+1)%100 == 0 ) printf(".");
						if( (i+1)%1000 == 0 ) printf("\n");
						continue;
					} else {
						printf("Memory error at location %d\n",i);
						exit(1);
					}
				}
				printf("done\n");
				printf("checking second pattern\n");
				basenumber = 3*JUMBOSIZE/N ; 
				printf("i from %d to %d\n",basenumber, basenumber + 2*JUMBOSIZE/N);
				for (unsigned  int i= basenumber; i < (basenumber+2*(JUMBOSIZE/N)); i ++) {
					if( array2[i-basenumber] == 
						((NUMBER2*(i-basenumber) + ((i-basenumber) % NUMBER3)))) {
							if( (i-basenumber) < (NUMBER1 +10)) {
								printf("B[%d] = %u shouldbe %u\n",i-basenumber,array2[i-basenumber], 
									((NUMBER2*(i-basenumber) + (i-basenumber) % NUMBER3)));
							}
						if( (i+1)%100 == 0 ) printf(".");
						if( (i+1)%1000 == 0 ) printf("\n");
						continue;
					}
					else {
							if( (i-basenumber) < (NUMBER1 +10)) {
								printf("B[%d] = %u shouldbe %u\n",i-basenumber,array2[i-basenumber], 
									((NUMBER2*(i-basenumber) + (i-basenumber) % NUMBER3)));
							}
						printf("error:Memory error at location %d\n",i);
						//exit(1);
					}
				printf("done\n");
				}

				fp =  fopen("/proc/self/status", "r");
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
				exit(0);

}
