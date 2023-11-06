#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/auxv.h>
#include <elf.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


#include "pager.h"


void displayaddressspace() {
    FILE *fp;
    char buffer[5020];
    fp = fopen("/proc/self/maps", "r");
    while (fgets(buffer, 5000, fp)) {
        fputs(buffer, stdout);
    }
    printf("\n");
    fclose(fp);
    return;
}

void *allocatestack(size_t size) {
    int stackprots = PROT_NONE;
    stackprots |= PROT_READ;
    stackprots |= PROT_WRITE;

    int stackflags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK |
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
    int padbytes = 16 - stackbytesneeded%16 ;
    stackbytesneeded += ( padbytes ) ;
    assert( (stackbytesneeded % 16 ) == 0);




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

    printf("%p:topofthestack:\n", topofthestack) ;
    printf("%p:stackargcptr:\n", stackargcptr);
    printf("%p:stackargvptr:	+(%d)\n" 	,  stackargvptr, argcbytes );
    printf("%p:stackenvptr: 	+(%d)\n"	,  stackenvptr, argvbytes) ;
    printf("%p:stackauxptr: 	+(%d)\n"  	,  stackauxptr, envptrbytes);
    printf("%p:stackargvstrptr: +(%d)\n", stackargvstrptr, auxbytes) ;
    printf("%p:stackenvstrptr: 	+(%d)\n",  stackenvstrptr, argvstrbytes );
    printf("%p:stackend:	+(%d)\n"   	,  stackend, envstrbytes );
    printf("%p:stackendpadded: 	+(%d)\n",  stackendpadded,padbytes);

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
        cptr += strlen(argv[i]) ;
        cptr++ ;

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
        eptr+=(strlen(*tempiter));
        eptr++ ;
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
    ----------------------------*/



    return topofthestack;
}


#ifdef DEMANDPAGING
int fd;
Elf64_Ehdr elfheader;
Elf64_Phdr *programs;
Elf_Auxv auxvector ;

#define MAX_AS_ENTRIES  10

// space to keep valid address spaces of the loaded executable
typedef struct {
	uint64_t pagestart ;
	uint64_t size ;
	int prots ;
	int flags ; 
	uint64_t fileoffset ; 
} ASEntry ; 

typedef struct {
	int count ; 
	ASEntry entries[MAX_AS_ENTRIES] ;
} ASTable ;

ASTable  addressspaces ; 

#define NEW_AS_ENTRY(masptr, mstart,msize,mprots,mflags,mfileoffset)		\
	do	{\
	(masptr)->entries[(masptr)->count].pagestart = (mstart)	;\
	(masptr)->entries[(masptr)->count].size = (msize)	;\
	(masptr)->entries[(masptr)->count].prots = (mprots)	;\
	(masptr)->entries[(masptr)->count].flags = (mflags)	;\
	(masptr)->entries[(masptr)->count].fileoffset = (mfileoffset)	;\
	(masptr)->count ++ ;\
	} while(0);

#define INIT_ASTABLE(masptr) 	\
	do 	{\
		for(int i = 0 ; i< MAX_AS_ENTRIES ; i++) { \
			(masptr)->entries[i].pagestart = 0 ; \
			(masptr)->entries[i].size = 0 ; \
			(masptr)->entries[i].prots = 0 ; \
			(masptr)->entries[i].flags = 0 ; \
			(masptr)->entries[i].fileoffset = 0 ; \
		} \
		(masptr)->count = 0 ;  \
	} while(0);


#define PRINT_ASTABLE(masptr) \
	do 	{\
		for(int i = 0 ; i< MAX_AS_ENTRIES ; i++) { \
			printf("Astable[%d] start:%lx, size:%lx, end:%lx, prots:%x, flags=%x, offset=%lx\n",\
			i,\
			(masptr)->entries[i].pagestart, \
			(masptr)->entries[i].size, \
			(masptr)->entries[i].pagestart + (masptr)->entries[i].size, \
			(masptr)->entries[i].prots ,	\
			(masptr)->entries[i].flags ,	\
			(masptr)->entries[i].fileoffset );	\
		} \
		printf("Total N = %d\n", (masptr)->count );  \
	} while(0);



static void demandpager(int sig, siginfo_t *si, void *contexts) {
	void *ret;

	char *whywhywhy = "undefined" ; 
	//printf("signal number: %d signo:%d\n",sig,si->si_signo);
	switch(si->si_code) {
		case SI_KERNEL: 
			whywhywhy = "Signal sent by Kernel";
			break ;
		case SI_USER: 
			whywhywhy = "Signal sent by User";
			break ;
		case SEGV_MAPERR: 
			whywhywhy = "SIGSEV: address not mapped ";
			break ;
		case SEGV_ACCERR:
			whywhywhy = "SIGSEV: invalidpermission for the mapped object";
			break ;
	}

	uint64_t faultingaddress = (uint64_t) si->si_addr ; 
	uint64_t  pagestart = (uint64_t)(faultingaddress) & ~PAGE_MASK ; 
	uint64_t  pageend   = ((uint64_t)(faultingaddress) + PAGE_SIZE ) & ~PAGE_MASK ;




	// Check if the faulting address in the AS tables if so mmap it else exit gracefully 
	//printf("Demand pager: fault address: %p, pagestart %lx, pageend %lx\n", (void *)faultingaddress, pagestart,pageend);
	
	ASEntry *asptr ;
	int found = -1 ;
	for ( int i = 0 ; i < addressspaces.count ; i ++) {
		asptr =  &addressspaces.entries[i] ;
		if( (faultingaddress >= (asptr -> pagestart)) && ( faultingaddress < (asptr->pagestart + asptr->size))) {
			found = i ; 
			break ;
		}
	}

	//printf("si->sicode: %d %s\n", si->si_code, whywhywhy);
	if( si->si_code != SEGV_MAPERR) {
		printf("exit(1):Not capable of handling any thing other than SEGV_MAPPER fault:%p\n", si->si_addr);
		//printf("   (found:%d)		pagestart = %lx, pageend = %lx\n", found,pagestart,pageend); 
		exit(1);
	}


	if( found == -1) {
		printf("Error:unhandled sigsegv fault at fault:%p\n, exiting...",si->si_addr);
		exit(1);
	}

	asptr =  &addressspaces.entries[found] ;


	uint64_t pageentrystart ;
	uint64_t pageentryoffset ;
	int fileprot ;
	int fileflags ;


	pageentrystart = pagestart ; 
	pageentryoffset = pagestart - asptr->pagestart + asptr->fileoffset ;
	fileprot = asptr->prots ; 
	fileflags = asptr->flags ; 


   	void * map_pointer ;


   	printf("demandpaging: entrypage mmap( start address=%lx end address=%lx mapsize=%lx prot=%x flags=%x fd=%d offset=%lx) \n",
                   	pageentrystart, pageentrystart+PAGE_SIZE, PAGE_SIZE, fileprot, fileflags, fd, pageentryoffset);

	map_pointer = mmap((void *)pageentrystart, (uint64_t)PAGE_SIZE, fileprot, fileflags, fd, pageentryoffset);
	if (map_pointer == MAP_FAILED) {
		perror("mmap()");
		fprintf(stderr,"program can not be loaded as there is a clash of address[%p:%p] program %d\n",
			(void *)pageentrystart, (void *)pageentrystart+PAGE_SIZE, found);
		close(fd);
		free(programs);
	
		exit(1);
	}
	printf("map_pointer: %p\n",map_pointer);

	//printf("\n-------\n");

	//TODo
	//exit(1);

}


#endif







int main(int argc, char **argv, char** envp) {
    int ret;
#ifdef DEMANDPAGING
#else
    int fd;
    Elf64_Ehdr elfheader;
    Elf64_Phdr *programs;
    Elf_Auxv auxvector ;
#endif


#ifdef DEMANDPAGING
// Define signal handler

 struct sigaction saofdemandpager;
 saofdemandpager.sa_flags = SA_SIGINFO;
 saofdemandpager.sa_sigaction = demandpager;
 sigemptyset(&saofdemandpager.sa_mask);
 if (sigaction(SIGSEGV, &saofdemandpager, NULL) == -1){
 	perror("sigaction()");	
	fprintf(stderr,"Setting the callback function for demandpaging failed\n");
 	exit(1) ;
 }
 
	/* TEST For sigsegv ( it comes correctly ) 
 	char *invalidptr = 0x0000100 ; 
 	*invalidptr = 45 ; 
	exit(1);
	*/




	INIT_ASTABLE(&addressspaces) ; 	

#else
#endif


    void *entry = NULL ;
    void *newsp = NULL  ;

    auxvector.auxcount = 0 ;
    memset(auxvector.auxentries, 0UL, sizeof(Elf_Aux)*MAX_AUXV);
    NEW_AUX_ENTRY(&auxvector,AT_EXECFN,(uint64_t)argv[1]);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <executable> <args> \n",argv[0]);
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

    /*
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%x,", elfheader.e_ident[i]);
    }
    printf("\n");
    printf("Etype is %x \n", elfheader.e_type);
    printf("Ephoff is %lx \n", elfheader.e_phoff);
    printf("Emachine is %x \n", elfheader.e_machine);
    printf("Ephoff is %lx \n", elfheader.e_phoff);
    printf("Eshoff is %lx \n", elfheader.e_shoff);
    printf("Eflags is %x \n", elfheader.e_flags);
    printf("Ephentsize is %x \n", elfheader.e_phentsize);
    printf("Ephnum is %x \n", elfheader.e_phnum);
    printf("Eshentsize is %x \n", elfheader.e_shentsize);
    printf("Ephnum is %x \n", elfheader.e_shnum);
    */
    printf("Eentry is %lx \n", elfheader.e_entry);

    if( elfheader.e_type == ET_EXEC ) {
        NEW_AUX_ENTRY(&auxvector,AT_ENTRY, elfheader.e_entry);
        NEW_AUX_ENTRY(&auxvector,AT_PHENT, elfheader.e_phentsize);
        NEW_AUX_ENTRY(&auxvector,AT_PHNUM, elfheader.e_phnum);
        entry = (void *)elfheader.e_entry ;
    }

    programs = malloc(elfheader.e_phnum * elfheader.e_phentsize);
    if (programs == NULL) {
        perror("Memory error during programs initialisation");
        return -1;
    }

    ret = lseek(fd, elfheader.e_phoff, SEEK_SET);
    if (ret < 0) {
        perror("Error in lseek for programs!");
        return -1;
    }
    n = read(fd, programs, elfheader.e_phentsize * elfheader.e_phnum);
    assert(n == elfheader.e_phentsize * elfheader.e_phnum);

    //displayaddressspace();

    for (int i = 0; i < elfheader.e_phnum; i++) {

        //printf("Details of %d program:", i);
        //printf("Ptype %x,", programs[i].p_type);
        //printf("Pflags %x,", programs[i].p_flags);
        //printf("Poffset %lx,", programs[i].p_offset);
        //printf("Pvaddr %lx,", programs[i].p_vaddr);
        //printf("Ppaddr %lx \n", programs[i].p_paddr);
        //printf("Pfilesz %lx,", programs[i].p_filesz);
        //printf("Pmemsz %lx,", programs[i].p_memsz);
        //printf("Palign %lx \n", programs[i].p_align);


        if (programs[i].p_type == PT_LOAD) {

#ifdef DEMANDPAGING
			// Check the page which has executable's entry 

			int rightprogram = 0; // if the current program is "interesting"
			int programtoload = -1 ; // index of "interesting" program 
			uint64_t pageentrystart ;
			uint64_t pageentryend ;
			uint64_t pageentryoffset ;

			do {
				uint64_t entry = elfheader.e_entry ; 
				uint64_t startoffset = programs[i].p_vaddr ; 
				uint64_t endoffset   = programs[i].p_vaddr + programs[i].p_filesz ; 


				int programtoload ; 

				printf("-->		Entry = %lx, start = %lx, end = %lx\n",entry,startoffset,endoffset);
				if( (entry >= startoffset ) && ( entry < endoffset )) rightprogram = 1 ; 
				if( rightprogram) {
					programtoload = i ; 
					printf("			-> Correct Program[%d] is %d\n",programtoload,rightprogram);
					pageentrystart = entry & ~PAGE_MASK ; 
					pageentryend   = ( entry + PAGE_SIZE) & ~PAGE_MASK ; 

					printf("-->		first page is ready for mmap ( address = %lx, size = %lx )\n", pageentrystart, pageentryend-pageentrystart );
				}
			}
			while(0);

#else
#endif



            uint64_t loadaddress = programs[i].p_vaddr;
            uint64_t filemapstart = loadaddress & ~PAGE_MASK;
            // getting the start of the region to map, aligned at 1000 boundary

            uint64_t filemapend = (loadaddress + programs[i].p_filesz + PAGE_SIZE) & ~PAGE_MASK;
            // getting the end of the region to map
            uint64_t filemapsize = filemapend - filemapstart;
            uint64_t filemappages = filemapsize / PAGE_SIZE;

            assert(filemappages * PAGE_SIZE == filemapsize);

            uint64_t filemapoffset = programs[i].p_offset & ~PAGE_MASK; // mmap offset has to be page aligned



            // TODO: check if virtual address clashes before mmap
            int fileprot = PROT_NONE;


            if (programs[i].p_flags & PF_W) {
                fileprot |= PROT_WRITE;
            }
            if (programs[i].p_flags & PF_R) {
                fileprot |= PROT_READ;
            }
            if (programs[i].p_flags & PF_X) {
                fileprot |= PROT_EXEC;
            }

            int fileflags = MAP_PRIVATE;
            fileflags |= MAP_POPULATE;
            fileflags |= MAP_FIXED_NOREPLACE;


            void *map_pointer;

#ifdef DEMANDPAGING
			/*
			for reference

			int rightprogram = 0; // if the current program is "interesting"
			int programtoload = -1 ; // index of "interesting" program 
			uint64_t pageentrystart ;
			uint64_t pageentryend ;
			uint64_t pageentryoffset ;
			*/

			if(rightprogram) {
				pageentryoffset = pageentrystart - filemapstart + filemapoffset ;

            	printf("demandpaging: entrypage mmap( start address=%lx end address=%lx mapsize=%lx prot=%x flags=%x fd=%d offset=%lx) \n",
                   	pageentrystart, pageentrystart+PAGE_SIZE, PAGE_SIZE, fileprot, fileflags, fd, pageentryoffset);

            	map_pointer = mmap((void *)pageentrystart, (uint64_t)PAGE_SIZE, fileprot, fileflags, fd, pageentryoffset);
            	if (map_pointer == MAP_FAILED) {
                	perror("mmap()");
                	fprintf(stderr,"program can not be loaded as there is a clash of address[%p:%p] program %d\n",
                        (void *)pageentrystart, (void *)pageentrystart+PAGE_SIZE, i);
                	close(fd);
                	free(programs);

                	exit(1);
				}
			}
			NEW_AS_ENTRY(&addressspaces, filemapstart,filemapsize,fileprot,fileflags,filemapoffset)	;	

#else
            printf("filemapped mmap( start address=%lx end address=%lx mapsize=%lx prot=%x flags=%x fd=%d offset=%lx) \n",
                   filemapstart, filemapend, filemapsize, fileprot, fileflags, fd, filemapoffset);

            map_pointer = mmap((void *)filemapstart, filemapsize, fileprot, fileflags, fd, filemapoffset);

            if (map_pointer == MAP_FAILED) {
                perror("mmap()");
                fprintf(stderr,"program can not be loaded as there is a clash of address[%p:%p] program %d\n",
                        (void *)filemapstart, (void *)filemapstart+filemapsize, i);
                close(fd);
                free(programs);

                exit(1);
            }
#endif


            if (programs[i].p_memsz > programs[i].p_filesz) {
                uint64_t anonmapstart = filemapend;
                uint64_t lastvirtualaddr = loadaddress + programs[i].p_memsz;

                // unsigned long anonmapsize=programs[i].p_memsz-filemapsize;
                uint64_t anonmapend = (lastvirtualaddr + PAGE_SIZE) & ~PAGE_MASK;
                uint64_t anonmapsize = anonmapend - anonmapstart;

                if (anonmapsize > 0) {

                    int anonprot = fileprot;

                    int anonflags = MAP_PRIVATE;
                    anonflags |= MAP_ANONYMOUS;
                    anonflags |= MAP_FIXED_NOREPLACE;
                    anonflags |= MAP_POPULATE;

                    printf("Anonymous  mmap( start address=%lx end address=%lx mapsize=%lx prot=%x flags=%x fd=%d offset=%lx) \n",
                           anonmapstart, anonmapend, anonmapsize, anonprot, anonflags, -1, 0L);

                    map_pointer = mmap((void *)anonmapstart, anonmapsize, anonprot, anonflags, -1, 0L);
                    if (map_pointer == MAP_FAILED) {
                        perror("Anon mmap()");
                        fprintf(stderr,"program can not be loaded as can not allocate memory for BSS [%p:%p] program %d\n",
                                (void *)anonmapstart, (void *)anonmapstart+anonmapsize, i);
                        close(fd);
                        free(programs);
                        exit(1);
                    }

#ifdef DEMANDPAGING
					// TODO how to handle this for demand paging 
#else
					// set the memory to zero for the all the bytes beyond filez and until memsz
                    if( anonprot & PROT_WRITE ) {
                        memset ( (void *) ( programs[i].p_vaddr + programs[i].p_filesz),0UL,
                                 (uint64_t) (anonmapstart - (programs[i].p_vaddr + programs[i].p_filesz)));

                    } //resetting to zero
#endif
                }// for anon
            }

            if( programs[i].p_offset == 0 ) {
                NEW_AUX_ENTRY(&auxvector,AT_PHDR,(filemapstart+elfheader.e_phoff));
            }
        } // PT_LOAD handing
    } // For each program
    printf("\n");

#ifdef DEMANDPAGING
	free(programs);
#else
    free(programs);
    close(fd);
#endif







	//TODO 
	//exit(1);

    //  Set up the stack



    if(0) {

        uint64_t	stackbytesrequired = 20*PAGE_SIZE ;
        void *stacklowest = allocatestack(stackbytesrequired);

        if (stacklowest == NULL) {
            printf("Stack not allocated! \n");
        } else {
            printf("after allocate:Value of stack pointer: %p : %p \n", stacklowest,stacklowest + stackbytesrequired);
        }

        void *topofthestack = stacklowest + stackbytesrequired  - 64 ; // 64 bytes is reserved for stack overflow


        //displayaddressspace();
        newsp= populatestack(topofthestack, argc, argv, envp); // return the current stack point ( pointing to argc )

        if (newsp == NULL) {
            printf("Stack not populated! \n");
        } else {
            printf("after populate: Value of stack pointer: %p\n", newsp );
        }

    } else {

        if(allocateandfillstack(argc, argv, envp, &auxvector, &newsp) < 0 ) {
            exit(1);
        }
        //printf("after populate: Value of stack pointer: %p\n", newsp );

        //displayaddressspace();

        if (newsp == NULL) {
            printf("Stack not populated! \n");
            exit(1);
        } else {
            printstack((char *) newsp);
        }
    }





    //displayaddressspace();
    stackcheck(newsp,argc-1,&argv[1]);

#ifdef DEMANDPAGING
	PRINT_ASTABLE(&addressspaces);
#endif


    //givecontrol(void *programentry, void *topofthestack) ;
    printf("program entry is %p \n", entry);
    printf("stackpointer  is %p \n",newsp);

    //givecontrol(entry, newsp) ;
    starttheprogram(entry, newsp) ;

}
void starttheprogram(void *programentry, void *topofthestack) {
    __asm__ __volatile__(
        "xor %%rax, %%rax\n"
        "xor %%rbx, %%rbx\n"
        "xor %%rcx, %%rcx\n"
        "xor %%rdx, %%rdx\n"
        "xor %%rsi, %%rsi\n"
        "xor %%rdi, %%rdi\n"
        "xor %%r8, %%r8\n"
        "xor %%r9, %%r9\n"
        "xor %%r10, %%r10\n"
        "xor %%r11, %%r11\n"
        "xor %%r12, %%r12\n"
        "xor %%r13, %%r13\n"
        "xor %%r14, %%r14\n"
        "xor %%r15, %%r15\n"
        :::);

    __asm__ __volatile__(
        "mov %0, %%rsp\n"
        ::"a"(topofthestack):);


    __asm__ __volatile__(
        "jmp *%0\n"
        ::"a"(programentry):);
}

void givecontrol(void *programentry, void *topofthestack) {
    __asm__ __volatile__(
        "mov $0, %%rax\n"
        "mov $0, %%rbx\n"
        "mov $0, %%rcx\n"
        "mov $0, %%rdx\n"
        "mov $0, %%rsi\n"
        "mov $0, %%rdi\n"
        "mov $0, %%r8\n"
        "mov $0, %%r9\n"
        "mov $0, %%r10\n"
        "mov $0, %%r11\n"
        "mov $0, %%r12\n"
        "mov $0, %%r13\n"
        "mov $0, %%r14\n"
        "mov $0, %%r15\n"
        :::);

    __asm__ __volatile__(
        "mov %0, %%rsp\n"
        ::"a"(topofthestack):);


    __asm__ __volatile__(
        "jmp *%0\n"
        ::"a"(programentry):);
}


