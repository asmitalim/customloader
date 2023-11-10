#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

#include "mallocfreefilter.h"



void free(void *freeptr) {
	static void (*bottomlayerfree)(void *) = NULL ; 
	
	if( bottomlayerfree == NULL ) {
		bottomlayerfree = dlsym(RTLD_NEXT,"free");
	}
	printf("freefilter:Trying to free memory \n");
	printf("freefilter:Freed pointer %p\n",freeptr);
	bottomlayerfree(freeptr) ;
}

void *malloc(size_t sizeneeded) {
	static void *(*bottomlayermalloc)(size_t ) =  NULL ;
	if(bottomlayermalloc == NULL ) {
		bottomlayermalloc = dlsym(RTLD_NEXT,"malloc");
	}
	printf("mallocfilter:Trying to allocate memory of size %ld\n",sizeneeded);
	char *allocatedptr = bottomlayermalloc(sizeneeded);
	if( allocatedptr == NULL) {
		perror("mallocfilter:libc allocate failed:");
		return NULL ;
	}
	else { 
		printf("mallocfilter:allocated pointer = %p\n", allocatedptr );
		return allocatedptr ; 
	}
}
