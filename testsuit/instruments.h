#ifndef INSTRUMENTS_H
#define INSTRUMENTS_H


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>
#include <ctype.h>
#include <stdint.h>


void initheaders(char *prefixheader);
void instrumentstats(char *prefixstring, int headerflag) ;


#endif
