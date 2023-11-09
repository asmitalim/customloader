#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>



size_t bytesAllocatedSofar = 0 ;



void free(void *ptrTobeFreed)
{
    static void    (*lowerLayerFree)   (void *freePtr) = NULL ;

	if( lowerLayerFree == NULL ) {
    	lowerLayerFree = dlsym(RTLD_NEXT,"free");
	}
    lowerLayerFree(ptrTobeFreed);
}


void *malloc(size_t bytesNeeded)
{
    static void *  (*lowerLayerMalloc) (size_t bytesRequired) = NULL ;
	if ( lowerLayerMalloc == NULL ) {
    	lowerLayerMalloc = dlsym(RTLD_NEXT,"malloc");
	}

    void *allocatedPtr = lowerLayerMalloc(bytesNeeded);
	memset(allocatedPtr,(int)0xCC,bytesNeeded);
    bytesAllocatedSofar += bytesNeeded ;
    fprintf(stderr,"Total bytes allocated so far %zd\n",bytesAllocatedSofar);
    return allocatedPtr ;
}


