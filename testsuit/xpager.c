#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#ifndef MAX_BUF
#define MAX_BUF 200
#else
#undef MAX_BUF
#define MAX_BUF 200
#endif


int main(int argc, char **argv, char *envp) {

	char path[MAX_BUF] ;
	char commandpath[MAX_BUF] ;
	printf("Built in loader\n");

	if( argc < 2 ) {
		fprintf(stderr, "Usage %s <executable> <arg> <arg>\n",argv[0]);
		exit(1);
	}

	if(getcwd(path,MAX_BUF)<0) { perror("getcwd");exit(1); }
	printf("path is %s\n",path);

	sprintf(commandpath,"%s/%s",path,argv[1]);
	printf("command path is %s\n",commandpath);


	if(execve(commandpath,&argv[1],envp) < 0) { perror("execve");exit(2); }
	printf("you will never reach here\n");
}
