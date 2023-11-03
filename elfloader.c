#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>
#include <assert.h>
#include <stdlib.h>
#include<string.h>
#define PF_X (1 << 0) /* Segment is executable */
#define PF_W (1 << 1) /* Segment is writable */
#define PF_R (1 << 2) /* Segment is readable */
#define PAGE_MASK 0xFFF
#define PAGE_SIZE 0x1000



void displayaddressspace() ;
void *allocatestack(size_t size) ;
void *populatestack(void* sp, int argc, char **argv, char **envp) ;
void stack_check(void* top_of_stack, uint64_t argc, char** argv) ;
int main(int argc, char **argv, char** envp) ;


void displayaddressspace() {
    FILE *fp;
    char buffer[5000];
    fp = fopen("/proc/self/maps", "r");
    while (fgets(buffer, 5000, fp)) {
        fputs(buffer, stdout);
    }
    printf("\n");
    return;
}

void *allocatestack(size_t size) {
    unsigned long stackprots = PROT_NONE;
    stackprots |= PROT_READ;
    stackprots |= PROT_WRITE;

    unsigned long stackflags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK |
                               MAP_GROWSDOWN | MAP_POPULATE;
    void *stackpointer = mmap(NULL, size, stackprots, stackflags, -1, 0L);
    if (stackpointer == MAP_FAILED) {
        perror("Mmap of stack!  \n");
        return NULL;
    }
    return stackpointer ;
}






void* populatestack(void* passedtopofthestack, int argc, char **argv, char **envp) {
    /*
    ** algo

    			passedtopofthestack  is current stack pointing to nowhere.

    			- calculate the size of stack required
    					argcBytes
    			- pad appropriately so that the stack is 8 bytes aligned

    **
    **
    */
    long stackbytesneeded=0;

    /*
    ** print, calculate bytes, calculate pointers, display needed
    	- argc,
    	- argv pointers,
    	- envp pointers,
    	- aux values,
    	- argv strings
    	- envp strings
    */



    printf("Argc value is : %d \n", argc);
    int argcbytes;

    argcbytes = sizeof(uint64_t);
    stackbytesneeded += argcbytes;

    printf("After argc(%d): Stack bytes needed: %ld \n", argcbytes,stackbytesneeded);







    int argvbytes = 0;
    int argvstrbytes = 0;
    for (int i = 0; i < argc; i++) {
        printf("Argv[%d] is %s \n", i, argv[i]);

        argvstrbytes += (strlen(argv[i])+1);
        argvbytes += sizeof(char  *);
    }
    argvbytes += sizeof(char  *);
    stackbytesneeded+= (argvbytes + argvstrbytes);

    printf("After argv(%d), argvs(%d): Stack bytes needed: %ld \n", argvbytes,argvstrbytes, stackbytesneeded);








    char **iter = envp;
    int envcount=0;
	int envptrbytes = 0;
	int envstrbytes = 0; 
    for (; *iter != NULL; iter++) {
        printf("Env variables value is %s \n", *iter);

		envptrbytes	+= sizeof(char *);
		envstrbytes += (strlen(*iter)+1);
        envcount++;
    }
	envptrbytes	+= sizeof(char *); // for null terminated envp

    stackbytesneeded += (envptrbytes + envstrbytes);

    printf("After envp:ptr*%d=(%d) str(%d) Stackbytes needed: %ld \n", envcount,envptrbytes,envstrbytes,stackbytesneeded);








    iter++; // advance the last null pointer. 


	//  Aux 


    Elf64_auxv_t *auxptr;
    auxptr = (Elf64_auxv_t *)iter;
    int 	auxcount=0;
	int  	auxbytes=0;

    for (; auxptr->a_type != AT_NULL; auxptr++) {
        printf("Aux contents are type:%lx and value:%lx \n", auxptr->a_type, auxptr->a_un.a_val);
        auxcount++;
		auxbytes += sizeof(Elf64_auxv_t) ; 
    }
	auxbytes += sizeof(Elf64_auxv_t) ; // add a last entry which has a type AT_NULL

    stackbytesneeded+= ( auxbytes ) ; 
    printf("After auxv:Total Auxs:%d, auxbytes:%d,  Stack bytes needed: %ld \n", auxcount, auxbytes,stackbytesneeded);


	// example  bytesneed = 53 , bytes required to be 8 bytes aligned 
	int padbytes = 8 - stackbytesneeded%8 ; 
    stackbytesneeded += ( padbytes ) ; 
	assert( (stackbytesneeded % 8 ) == 0);



	
	void *topofthestack = passedtopofthestack - stackbytesneeded  ;


	void  *stackptr = topofthestack ; // we do not want to disturb topofthestack ;


	assert( stackbytesneeded == (argcbytes+argvbytes + envptrbytes + auxbytes + argvstrbytes + envstrbytes + padbytes ));
	// argc 
	void *stackargcptr 		= topofthestack ; 
	void *stackargvptr 		= stackargcptr + argcbytes ; 
	void *stackenvptr  		= stackargvptr + argvbytes ;  
	void *stackauxptr  		= stackenvptr + envptrbytes;
	void *stackargvstrptr 	= stackauxptr + auxbytes ; 
	void *stackenvstrptr 	= stackargvstrptr + argvstrbytes ; 
	void *stackend   		= stackenvstrptr + envstrbytes ; 
	void *stackendpadded 	= stackend + padbytes ; 


	printf("\n\n\n");
	printf("passedtopofthestack %p\n",passedtopofthestack);
	printf("stackbytesneed %ld\n",stackbytesneeded);
	printf("padbytes %d\n",padbytes);
	printf("\n\n");

	printf("%p:topofthestack:\n" , topofthestack) ; 
	printf("%p:stackargcptr:\n" , stackargcptr);
	printf("%p:stackargvptr:	+(%d)\n" 	,  stackargvptr , argcbytes ); 
	printf("%p:stackenvptr: 	+(%d)\n"	,  stackenvptr , argvbytes) ;  
	printf("%p:stackauxptr: 	+(%d)\n"  	,  stackauxptr, envptrbytes);
	printf("%p:stackargvstrptr: +(%d)\n" , stackargvstrptr , auxbytes) ; 
	printf("%p:stackenvstrptr: 	+(%d)\n" ,  stackenvstrptr , argvstrbytes ); 
	printf("%p:stackend:	+(%d)\n"   	,  stackend , envstrbytes ); 
	printf("%p:stackendpadded: 	+(%d)\n" ,  stackendpadded,padbytes);

	assert ( stackendpadded == passedtopofthestack ) ;


	// Second pass ( now the layout is ready so populate the stack 

	// copy the argc on the stack 
	*((uint64_t *)stackargcptr) = (uint64_t) argc ; 



	// copy the argv on the stack

	char **tempargv = (char **)stackargvptr ; 
	char *tempargvstr = (char *)stackargvstrptr ;

	char *cptr = (char *)stackargvstrptr ; 
	int  nargvbytes =0;
    for (int i = 0; i < argc; i++) {
        printf("copying Argv[%d] is %s \n", i, argv[i]);
		strcpy(cptr,argv[i]);

		tempargv[i] = cptr ; 
		cptr += strlen(argv[i]) ; cptr++ ; 

        nargvbytes += (strlen(argv[i])+1);
    }
	tempargv[argc] = (char *) NULL ; 

	// validate the argv


	do {
		char **testargv = (char **)stackargvptr ; 
		int i ;
		for (i= 0; i < argc ; i ++) {
			printf("argv[%d] 's %p  value [%s]\n",i,testargv[i],testargv[i] );
		}
		printf("argv[%d] 's %p  value [%s]\n",i,testargv[i],"" );

	} while(0);


	// copy the environment pointers and strings  on the stack 


    char **tempiter = envp;

	char **tempenv = (char **)stackenvptr ; 
	char *tempenvstr = (char *)stackenvstrptr ;

	char *eptr = (char *)stackenvstrptr ; 
	int  nenvbytes =0;

	int envindex = 0 ; 
    for (; *tempiter != NULL; tempiter++) {
        printf("copying Env variables value is %s \n", *tempiter);
		strcpy(eptr,*tempiter);
		tempenv[envindex] = eptr ; 
		nenvbytes += (strlen(*tempiter)+1);
		eptr+=(strlen(*tempiter)); eptr++ ;
		envindex ++ ; 
    }
	tempenv[envindex] = (char *)NULL ; 

	// validate the envp
	do {
		char **testenv = (char **)stackenvptr ; 
		int i ;
		for (i= 0; i < envindex ; i ++) {
			printf("envp[%d] 's %p  value [%s]\n",i,testenv[i],testenv[i] );
		}
		printf("envp[%d] 's %p  value [%s]\n",i,testenv[i],"" );
	} while(0);





	tempiter++ ; //for aux 

	// copy Aux to stack

	
    Elf64_auxv_t *tempauxptr;
    tempauxptr = (Elf64_auxv_t *)tempiter;

	Elf64_auxv_t *aptr = (Elf64_auxv_t *) stackauxptr ; 


	int tempauxcount  = 0 ; 
	int tempauxbytes = 0 ;
    for (; tempauxptr->a_type != AT_NULL; aptr++,tempauxptr++) {
        printf("Aux contents copied to stack  type:%lx and value:%lx \n", tempauxptr->a_type, tempauxptr->a_un.a_val);

		aptr->a_type = tempauxptr->a_type ;
		aptr->a_un.a_val = tempauxptr->a_un.a_val ;

        tempauxcount++;
		tempauxbytes += sizeof(Elf64_auxv_t) ; 
    }
	aptr->a_type = tempauxptr->a_type ;
	aptr->a_un.a_val = tempauxptr->a_un.a_val ;
	tempauxbytes += sizeof(Elf64_auxv_t) ; 

	assert(tempauxbytes == auxbytes );


	do {
		Elf64_auxv_t *testaux = (Elf64_auxv_t *)stackauxptr ; 
		int i ;
		for (i= 0; i <= tempauxcount ; testaux++,i ++) {
        	printf("auxv[%d]:type:%lx and value:%lx \n", i,testaux->a_type, testaux->a_un.a_val);
		}
	} while(0);




	/*------------------- copy pasted 
    Elf64_auxv_t *auxptr;
    auxptr = (Elf64_auxv_t *)iter;
    int 	auxcount=0;
	int  	auxbytes=0;

    for (; auxptr->a_type != AT_NULL; auxptr++) {
        printf("Aux contents are type:%lx and value:%lx \n", auxptr->a_type, auxptr->a_un.a_val);
        auxcount++;
		auxbytes += sizeof(Elf64_auxv_t) ; 
    }
	auxbytes += sizeof(Elf64_auxv_t) ; // add a last entry which has a type AT_NULL

	----------------------------*/



    return topofthestack;
}




/**
 * Routine for checking stack made for child program.
 * top_of_stack: stack pointer that will given to child program as %rsp
 * argc: Expected number of arguments
 * argv: Expected argument strings
 */
void stack_check(void* top_of_stack, uint64_t argc, char** argv) {
	printf("----- stack check -----\n");

	assert(((uint64_t)top_of_stack) % 8 == 0);
	printf("top of stack is 8-byte aligned\n");


	uint64_t* stack = top_of_stack;
	uint64_t actual_argc = *(stack++);
	printf("argc: %lu\n", actual_argc);
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
	printf("----- end stack check -----\n");
	//TODO
	return ; 
}






int main(int argc, char **argv, char** envp) {
    int fd;
    int ret;
    Elf64_Ehdr elfheader;
    Elf64_Phdr *programs;

    if (argc < 2) {
        fprintf(stderr, "Give the ELF file as argument!");
        return -1;
    }
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error in opening file!");
        return -1;
    }
    ret = lseek(fd, 0L, SEEK_SET);
    if (ret < 0) {
        perror("Error in lseek for elf_header!");
        return -1;
    }
    int n = read(fd, &elfheader, sizeof(elfheader));
    assert(n == sizeof(elfheader));
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%x,", elfheader.e_ident[i]);
    }
    printf("\n");
    /*
    printf("Etype is %x \n", elfheader.e_type);
    printf("Emachine is %x \n", elfheader.e_machine);
    printf("Eentry is %lx \n", elfheader.e_entry);
    printf("Ephoff is %lx \n", elfheader.e_phoff);
    printf("Eshoff is %lx \n", elfheader.e_shoff);
    printf("Eflags is %x \n", elfheader.e_flags);
    printf("Ephentsize is %x \n", elfheader.e_phentsize);
    printf("Ephnum is %x \n", elfheader.e_phnum);
    printf("Eshentsize is %x \n", elfheader.e_shentsize);
    printf("Ephnum is %x \n", elfheader.e_shnum);
    */

    programs = calloc(elfheader.e_phnum, sizeof(Elf64_Phdr));
    if (programs == NULL) {
        perror("Memory error during programs initialisation");
        return -1;
    }

    ret = lseek(fd, elfheader.e_phoff, SEEK_SET);
    if (ret < 0) {
        perror("Error in lseek for programs!");
        return -1;
    }
    n = read(fd, programs, sizeof(Elf64_Phdr) * elfheader.e_phnum);
    assert(n == sizeof(Elf64_Phdr) * elfheader.e_phnum);

    displayaddressspace();

    for (int i = 0; i < elfheader.e_phnum; i++) {
        printf("Details of %d program:", i);
        printf("Ptype %x,", programs[i].p_type);
        printf("Pflags %x,", programs[i].p_flags);
        printf("Poffset %lx,", programs[i].p_offset);
        printf("Pvaddr %lx,", programs[i].p_vaddr);
        // printf("Ppaddr %lx \n", programs[i].p_paddr);
        printf("Pfilesz %lx,", programs[i].p_filesz);
        printf("Pmemsz %lx,", programs[i].p_memsz);
        printf("Palign %lx \n", programs[i].p_align);
        if (programs[i].p_type == PT_LOAD) { // executable program
            unsigned long loadaddress = programs[i].p_vaddr;
            unsigned long filemapstart = loadaddress & ~PAGE_MASK;
            // getting the start of the region to map, aligned at 1000 boundary

            unsigned long filemapend = (loadaddress + programs[i].p_filesz + PAGE_SIZE) & ~PAGE_MASK;
            // getting the end of the region to map
            unsigned long filemapsize = filemapend - filemapstart;
            unsigned long filemappages = filemapsize / PAGE_SIZE;
            assert(filemappages * PAGE_SIZE == filemapsize);
            unsigned long filemapoffset = programs[i].p_offset & ~PAGE_MASK; // mmap offset has to be page aligned
            // TODO: check if virtual address clashes before mmap
            unsigned long fileprot = PROT_NONE;
            if (programs[i].p_flags & PF_W) {
                fileprot |= PROT_WRITE;
            }
            if (programs[i].p_flags & PF_R) {
                fileprot |= PROT_READ;
            }
            if (programs[i].p_flags & PF_X) {
                fileprot |= PROT_EXEC;
            }
            unsigned long fileflags = MAP_PRIVATE;
            fileflags |= MAP_FIXED_NOREPLACE;
            fileflags |= MAP_POPULATE;

            printf("Mmap start address=%lx end address=%lx mapsize=%lx prot=%lx flags=%lx fd=%d offset=%lx \n",
                   filemapstart, filemapend, filemapsize, fileprot, fileflags, fd, filemapoffset);
            void *map_pointer;
            map_pointer = mmap((void *)filemapstart, filemapsize, fileprot, fileflags, fd, filemapoffset);
            if (map_pointer == MAP_FAILED) {
                perror("Mmap!");
                printf("Map failed for region %d", i);
            }

            if (programs[i].p_memsz > programs[i].p_filesz) {
                unsigned long anonmapstart = filemapend;
                unsigned long lastvirtualaddr = loadaddress + programs[i].p_memsz;

                // unsigned long anonmapsize=programs[i].p_memsz-filemapsize;
                unsigned long anonmapend = (lastvirtualaddr + PAGE_SIZE) & ~PAGE_MASK;
                unsigned long anonmapsize = anonmapend - anonmapstart;
                if (anonmapsize > 0) {
                    // do mmap
                    unsigned long anonprot = fileprot;
                    unsigned long anonflags = MAP_PRIVATE;
                    anonflags |= MAP_ANONYMOUS;
                    anonflags |= MAP_FIXED_NOREPLACE;
                    anonflags |= MAP_POPULATE;
                    printf("Anonymous map start address=%lx end address=%lx mapsize=%lx prot=%lx flags=%lx fd=%d offset=%lx \n",
                           anonmapstart, anonmapend, anonmapsize, anonprot, anonflags, -1, 0L);

                    map_pointer = mmap((void *)anonmapstart, anonmapsize, anonprot, anonflags, -1, 0L);
                    if (map_pointer == MAP_FAILED) {
                        perror("Anon map!");
                        printf("Anon map failed for region %d", i);
                    }
                }
            }
            printf("\n");
        }
    }


	uint64_t	stackbytesrequired = 20*PAGE_SIZE ; 


    void *stacklowest = allocatestack(stackbytesrequired);
    if (stacklowest == NULL) {
        printf("Stack not allocated! \n");
    } else {
        printf("after allocate:Value of stack pointer: %p : %p \n", stacklowest,stacklowest + stackbytesrequired);
    }
	
	void *topofthestack = stacklowest + stackbytesrequired  - 64 ; // 64 bytes is reserved for stack overflow


    displayaddressspace();

    void* newsp= populatestack(topofthestack, argc, argv, envp); // return the current stack point ( pointing to argc ) 
	
    if (newsp == NULL) {
        printf("Stack not populated! \n");
    } else {
        printf("after populate: Value of stack pointer: %p\n", newsp );
    }

	stack_check(newsp,argc,argv);

}
