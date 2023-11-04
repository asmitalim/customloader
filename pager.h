#ifndef ELFLOADER_H

#define ELFLOADER_H




#define PAGE_MASK ((uint64_t)0x00000FFF)
#define PAGE_SIZE ((uint64_t)0x00001000)


#define MAX_AUXV 64
typedef struct {
    uint64_t id;
    uint64_t val;
} Elf_Aux;

typedef struct {
    int auxcount;
    Elf_Aux auxentries[MAX_AUXV];
} Elf_Auxv;

#define NEW_AUX_ENTRY(auxentryptr,auxtype,auxval) \
     do { \
         (auxentryptr)->auxentries[(auxentryptr)->auxcount].id = (auxtype); \
         (auxentryptr)->auxentries[(auxentryptr)->auxcount].val = (auxval); \
         (auxentryptr)->auxcount++; \
     } while(0)


#define STACK_PUSH_U64(stackptr, bits64) \
      do { \
          (stackptr) = ((void *)(stackptr)) - sizeof (uint64_t); \
          *((uint64_t *)(stackptr)) = (uint64_t) (bits64); \
      } while(0)

#define STACK_PUSH_AUX(stackptr, auxid, auxval) \
      do { \
          STACK_PUSH_U64(stackptr, auxval); \
          STACK_PUSH_U64(stackptr, auxid); \
      } while(0)


void displayaddressspace() ;
void *allocatestack(size_t size) ;
void *populatestack(void* sp, int argc, char **argv, char **envp) ;
void stackcheck(void* top_of_stack, uint64_t argc, char** argv) ;

void givecontrol(void *programentry, void *topofthestack) ;
void starttheprogram(void *programentry, void *topofthestack) ;



void printstack(char *stackptr) ;

int allocateandfillstack(int argc, char **argv, char **envp, Elf_Auxv *auxvptr, void **topofstackptr) ;




#endif
