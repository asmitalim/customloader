#ifndef MALLOCFREEFILTER_H
#define MALLOCFREEFILTER_H

#define _GNU_SOURCE
#include <dlfcn.h>

void free(void *ptr) ;
void *malloc(size_t sizeneeded) ;

#endif
