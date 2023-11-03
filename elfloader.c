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






void* populatestack(void* sp, int argc, char **argv, char **envp) {
    /*
    ** algo

    			sp  is current stack pointing to nowhere.

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


    return sp;
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
        printf("Value of stack pointer: %p : %p \n", stacklowest,stacklowest + stackbytesrequired);
    }
	
	void *topofthestack = stacklowest + stackbytesrequired  - 64 ; // 64 bytes is reserved for stack overflow


    displayaddressspace();

    void* newsp= populatestack(topofthestack, argc, argv, envp); // return the current stack point ( pointing to argc ) 
	
    if (newsp == NULL) {
        printf("Stack not populated! \n");
    } else {
        printf("Value of stack pointer: %p\n", newsp );
    }

}
