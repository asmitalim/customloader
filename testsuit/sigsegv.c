#include <unistd.h>

int data[10] ;
int main() {
	data[12] = 15 ; 
	int *zero = NULL ; 
	return *zero ; 
}
