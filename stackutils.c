#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/auxv.h>
#include <sys/mman.h>
#include <elf.h>
#include <assert.h>



#include "pager.h"





void printstack(char *topofstack) {
    printf("%p: Argc:%ld\n", topofstack,*((uint64_t *)topofstack)) ;
    topofstack += 8;


    printf("\n");
    printf("%p: Argv:\n", topofstack);

    char **tempargv = (char **)topofstack ;
    int i = 0 ;
    while (*topofstack) {
        printf("		argv[%d]:[%s]\n", i,tempargv[i]) ;
        i++;
        topofstack += 8;
    }
    topofstack += 8;

    char **tempenvp = (char **)topofstack ;
    char **envptr = tempenvp;

    printf("\n");
    printf("%p: Envp:\n", topofstack);
    while (*envptr) {
        printf(" 			envp:%s\n", *envptr);
        envptr++;
        topofstack += 8;
    }
    topofstack += 8;

    printf("\n");
    printf("%p: Aux Value:\n", topofstack);

    Elf64_auxv_t *tempauxs = (Elf64_auxv_t *) topofstack;

    i = 0 ;
    while ( tempauxs -> a_type != AT_NULL ) {
        printf("%p:			Aux[%02d]:  type:%03ld   value:%lx\n",tempauxs,i,tempauxs->a_type, tempauxs->a_un.a_val );
        tempauxs++ ;
        i++ ;
    }
    printf("\n");

}



int allocateandfillstack(int argc, char **argv, char **envp, Elf_Auxv *auxv, void **topofstackptr) {
    char *stackptr;
    void *stackhighestaddress;
    void *stackallocated ;

    uint64_t stacksize = PAGE_SIZE*8;

    int stackflags = MAP_PRIVATE ;
    stackflags |= MAP_ANON;
    stackflags |= MAP_STACK;
    stackflags |= MAP_GROWSDOWN;
    stackflags |= MAP_POPULATE;

    int stackprot = PROT_READ|PROT_WRITE;

    if ((stackallocated = mmap(0, stacksize, stackprot, stackflags, -1, 0)) == MAP_FAILED) {
        perror("mmap() ");
        fprintf(stderr,"Stack allocation error, exiting (requested size:%ld)\n",stacksize);
        return -1;
    }
    stackhighestaddress = stackallocated + stacksize;
    stackptr = stackhighestaddress;





    int envcount = 0;

    char **argvptr, **envptr;

    // calculate the number of envp
    char **eptr = envp ;
    while(*eptr++) envcount++ ;

    envptr = malloc(sizeof (char *) * envcount);




    // copy the environment on the stack
    for (int i = 0; i < envcount; i++) {
        stackptr -= strlen(envp[i]) ;
        stackptr --; // null character skip
        strcpy(stackptr, envp[i]);
        envptr[i] = stackptr; // pointer to this string is
    }

    // allocate space and then copy the ( Note one extra less as we are not passing argv[0])
    argvptr = malloc(sizeof (char *) * (argc-1));
    for (int i = 1; i < argc; i++) {
        stackptr -= strlen(argv[i]) ;
        stackptr -- ; // null character skip
        strcpy(stackptr, argv[i]);
        argvptr[i-1] = stackptr; // pointer to this string is saved in malloc
    }

    // 16 bytes of random values
    for (int i = 0; i < 16; i++) {
        *--stackptr = rand();
    }
    char *randombytesptr = stackptr;

    // align  the stack to to 16 bytes boundary
    stackptr = (char *) ((uint64_t) stackptr &  ~0xfUL);


    // so far the stackptr is aligned with 16

    assert(( ((uint64_t)stackptr) % 16 ) == 0 );

    // Pass a new set of Aux to the loaded file
    NEW_AUX_ENTRY(auxv,  AT_UID, getauxval(AT_UID));
    NEW_AUX_ENTRY(auxv,  AT_EUID, getauxval(AT_EUID));
    NEW_AUX_ENTRY(auxv,  AT_GID, getauxval(AT_GID));
    NEW_AUX_ENTRY(auxv,  AT_EGID, getauxval(AT_EGID));
    NEW_AUX_ENTRY(auxv,  AT_SYSINFO_EHDR, getauxval(AT_SYSINFO_EHDR));
    NEW_AUX_ENTRY(auxv,  AT_HWCAP, getauxval(AT_HWCAP));
    NEW_AUX_ENTRY(auxv,  AT_HWCAP2, getauxval(AT_HWCAP2));
    NEW_AUX_ENTRY(auxv,  AT_PAGESZ, getauxval(AT_PAGESZ));
    NEW_AUX_ENTRY(auxv,  AT_CLKTCK, getauxval(AT_CLKTCK));
    NEW_AUX_ENTRY(auxv,  AT_PLATFORM, getauxval(AT_PLATFORM));
    NEW_AUX_ENTRY(auxv,  AT_RANDOM, (uint64_t)randombytesptr);
    NEW_AUX_ENTRY(auxv,  AT_FLAGS, getauxval(AT_FLAGS));

    // (higher) address
    //			padding logic
    //
    //							1 pading uint64t depending making total uint64 evens.
    //							AT_NULL aux ( 2 uint64_t)
    //							auxs ( 2 uint64_t * auxcount
    //							null ( 1 pointer )
    //							envp ( envcount pointers )
    //							null ( 1 pointer )
    //							argv ( argc  pointers )
    //							argc ( 1 uint64_t)
    // (lower address)			stack aligned with  0x10 ( i.e last 4 bits are zero )

    //int args_cnt = (auxv->auxcount * 2) + envcount + argc + 4;
    //if (args_cnt % 2) STACK_PUSH_U64(stackptr, 0);

    int  	totalUint64s = 2 						// for AT_NULL
                           +  auxv->auxcount *2 		// 2 * auxcount
                           + 1 						// for null
                           + envcount 					// for envp
                           + 1 						// one null
                           + (argc-1) 					// for argv which is one less as we are not passing argv[0]
                           + 1  ;						// for argc

    //printf("Total totalUint64 expected %d\n",totalUint64s);

    if ( (totalUint64s % 2) == 0 ) {
        //printf("even:not adding pad cusion on 1 extra UINT64 as total uint is even %d\n",totalUint64s);
        // no padding uint64_t
    } else {
        STACK_PUSH_U64(stackptr, 0);
        //printf("odd:adding pad cusion of extra UINT64 as total uint is odd %d\n",totalUint64s);
        // odd numbers of uint64's so push one now.
    }












    //push aux including one for AT_NULL
    STACK_PUSH_AUX(stackptr, AT_NULL, 0);
    for (int i = 0; i < auxv->auxcount; i++) {
        STACK_PUSH_AUX(stackptr, auxv->auxentries[i].id, auxv->auxentries[i].val);
    }

    // push one null followed by N envpointers
    STACK_PUSH_U64(stackptr, 0);
    stackptr -= (  (sizeof(char *)) * envcount ) ;
    memcpy(stackptr, envptr, sizeof(char *) * (envcount));

    // push one null followed by (argc-1)  argv pointers ( loader's argv[0] is skipped )
    STACK_PUSH_U64(stackptr, 0);
    stackptr -= (  (sizeof(char *)) * (argc-1)  );
    memcpy(stackptr, argvptr, sizeof(char *) * (argc-1));

    // finally  push a a space for argc  and make the stack pointer pointing to integer
    STACK_PUSH_U64(stackptr, 0);
    *((int *) stackptr) = argc - 1;

    //printstack(stackptr);

    assert (( ((uint64_t)(stackptr)) % 16) == 0 );

    *topofstackptr = stackptr;

    free(envptr);
    free(argvptr);

    return 0;
}




/**
 * Routine for checking stack made for child program.
 * top_of_stack: stack pointer that will given to child program as %rsp
 * argc: Expected number of arguments
 * argv: Expected argument strings
 */
void stackcheck(void* top_of_stack, uint64_t argc, char** argv) {
    printf("\n\n----- stack check -----\n");

    assert(((uint64_t)top_of_stack) % 8 == 0);
    printf("top of stack is 8-byte aligned\n");


    uint64_t* stack = top_of_stack;
    uint64_t actual_argc = *(stack++);
    printf("argc: %lu \n", actual_argc);
    printf("argc passed: %lu \n", argc);
    assert(actual_argc == argc);

    for (int i = 0; i < argc; i++) {
        char* argp = (char*)*(stack++);
        assert(strcmp(argp, argv[i]) == 0);
        printf("arg %d: %s\n", i, argp);
    }




    // Argument list ends with null pointer
    assert(*(stack++) == 0);


    int envp_count = 0;
    while (*(stack++) != 0)
        envp_count++;

    printf("env count: %d\n", envp_count);


    Elf64_auxv_t* auxv_start = (Elf64_auxv_t*)stack;
    Elf64_auxv_t* auxv_null = auxv_start;
    while (auxv_null->a_type != AT_NULL) {
        auxv_null++;
    }
    printf("aux count: %lu\n", auxv_null - auxv_start);
    printf("----- end stack check -----\n\n");
    //TODO
    return ;
}


