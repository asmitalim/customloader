#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SIZE 64

#define MAGIC 0xCC


int main()
{
    unsigned char *mem = malloc(SIZE);
    for( int i = 0 ; i < SIZE ; i++)
        printf("0x%02x ",mem[i] );
}
