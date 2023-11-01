all: toload elfloader

toload: toload.o
	gcc -static -o toload toload.o

elfloader: elfloader.o
	#gcc -static -Wl,-Ttext=0x8090000,--verbose -o elfloader elfloader.o
	gcc -static -Wl,--script=customlinkerfile,--verbose -o elfloader elfloader.o

.PHONY: all clean

.c.o: 
	gcc -c $<

clean:
	rm -f toload
	rm -f *.o
	rm -f elfloader

