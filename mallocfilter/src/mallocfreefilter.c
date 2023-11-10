#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

//include "mallocfreefilter.h"



void free(void *freeptr)
{
    void (*bottomlayerfree)(void *) = NULL ;

    if( bottomlayerfree == NULL ) {
    	bottomlayerfree = dlsym(RTLD_NEXT,"free");
    }
    fprintf(stderr,"freefilter:Trying to free memory \n");
    bottomlayerfree(freeptr) ;
    fprintf(stderr,"freefilter:Freed pointer %lx\n",(size_t)freeptr);
}

void *malloc(size_t sizeneeded)
{

    void *(*bottomlayermalloc)(size_t bytesneeded ) =  NULL ;

    if(bottomlayermalloc == NULL ) {
    	bottomlayermalloc = dlsym(RTLD_NEXT,"malloc");
    }

    fprintf(stderr,"mallocfilter:Trying to allocate memory of size %ld\n",sizeneeded);
    char *allocatedptr = bottomlayermalloc(sizeneeded);

    if( allocatedptr == NULL) {
        perror("mallocfilter:libc allocate failed:");
        fprintf(stderr, "can not allocate memory\n");
        return NULL ;
    } else {
        fprintf(stderr,"mallocfilter:allocated pointer = %lx\n", (size_t)allocatedptr );
        return allocatedptr ;
    }
}
