#include <stdio.h>
#include <string.h>
#include "mallocfreefilter.h"


int main() {
	int psize = 100 ; 
	int qsize = 5000 ; 
	int rsize  = 50000 ; 

	char *p = malloc(psize);
	char *q = malloc(qsize);
	char *r = malloc(rsize);

	memset(p, 'p', psize-1);
	memset(q, 'q', qsize-1);
	memset(r, 'r', rsize-1);

	p[psize-1] = '\0' ;
	q[qsize-1] = '\0' ;
	r[rsize-1] = '\0' ;
	p[10] = 'A' ; q[10] = 'B' ; r[10] = 'C' ;


	printf("pee = %s\n",p);
	printf("queue = %s\n",q);
	printf("are = %s\n",r);


	free(p);
	free(q);
	free(r);
}
