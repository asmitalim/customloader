#include <stdio.h>
#include "instruments.h"
int main(int argc, char **argv, char **envp) {

    printf("hello!! UT Austin\n");

    printf("argc %d\n", argc);

    for( int i = 0 ; i < argc ; i ++) {
        printf("argv[%d] =  %s\n",i,argv[i]);
    }

	initheaders("title");
	instrumentstats("Hello",1);
    return 0 ;
}
