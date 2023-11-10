#include<stdio.h>
#include<stdlib.h>
#include<elf.h>

#include "instruments.h"

int main(int argc, char* argv[], char* envp[]) {
    FILE* fp;
    char buffer[5000];
    printf("Argc value is : %d \n", argc);
    for(int i=0; i<argc; i++) {
        printf("Argv[%d] is %s \n", i, argv[i]);

    }
    char** iter=envp;
    for(; *iter!=NULL; iter++) {
        printf("Env variables value is %s \n", *iter);
    }
    iter++;
    Elf64_auxv_t* auxptr;
    auxptr= (Elf64_auxv_t*) iter;
    for(; auxptr->a_type!=AT_NULL; auxptr++) {
        printf("Aux contents are type:%lx and value:%lx \n", auxptr->a_type, auxptr->a_un.a_val);
    }
    printf("%s \n", getenv("SHELL"));
    fp=fopen("/proc/self/maps", "r");
    while(fgets(buffer,5000,fp)) {
        fputs(buffer,stdout);
    }
	instrumentstats("Argc, Argv, envp and Auxv Display");
    return 0;

}
