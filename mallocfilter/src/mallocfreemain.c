#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mallocfreefilter.h"


int main(int argc, char **argv, char **envp)
{
    size_t psize = 10 ;
    size_t qsize = 50 ;
    size_t rsize  = 500;

    char *p = malloc(psize);
    char *q = malloc(qsize);
    char *r = malloc(rsize);
    int pi = 'p' ;
    int qu = 'q' ;
    int ar = 'r' ;

    memset(p, pi, psize-2);
    memset(q, qu, qsize-2);
    memset(r, ar, rsize-2);

    p[psize-1] = '\0' ;
    q[qsize-1] = '\0' ;
    r[rsize-1] = '\0' ;
    p[3] = 'A' ;
    q[3] = 'B' ;
    r[3] = 'C' ;


    printf("pee = %s\n",p);
    printf("queue = %s\n",q);
    printf("are = %s\n",r);


    free(p);
    free(q);
    free(r);

    exit(0);
}
