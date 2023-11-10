all: apager  apager.ammi dpager hpager
	@echo "apager Built"
	cd testsuit ; make
	cd mallocfilter/src ; make

toload: toload.o
	gcc -static -Wl,-z,norelro -o toload toload.o

hello.static:	hello.o
	gcc -static -Wl,-z,norelro  -o hello.static hello.o
	@nm hello.static | sort > hellosymbols.txt

hello.dyn:	hello.o
	gcc -o hello.dyn hello.o


apager.ammi: apager.o stackutils.o pager.h
	@#gcc -static -Wl,-Ttext=0x8090000,--verbose -o apager apager.o
	@#gcc -static -Wl,--verbose -o apager apager.o
	gcc -static -Wl,--script=customlinkerfile -o apager.ammi apager.o stackutils.o

apager: apager.o stackutils.o pager.h
	@#gcc -static -Wl,-Ttext=0x8090000,--verbose -o apager apager.o
	@#gcc -static -Wl,--verbose -o apager apager.o
	gcc -static -Wl,--script=buntzlinkerfile -o apager apager.o stackutils.o

dpager: dpager.o stackutils.o pager.h
	@#gcc -static -Wl,-Ttext=0x8090000,--verbose -o apager apager.o
	@#gcc -static -Wl,--verbose -o apager apager.o
	gcc -static -Wl,--script=buntzlinkerfile -o dpager dpager.o stackutils.o

hpager: hpager.o stackutils.o pager.h
	@#gcc -static -Wl,-Ttext=0x8090000,--verbose -o apager apager.o
	@#gcc -static -Wl,--verbose -o apager apager.o
	gcc -static -Wl,--script=buntzlinkerfile -o hpager hpager.o stackutils.o

dpager.o:	apager.c pager.h
	gcc -DDEMANDPAGING -g -c -o dpager.o apager.c

hpager.o:	apager.c pager.h
	gcc -DDEMANDPAGING -DHYBRIDPAGING -g -c -o hpager.o apager.c

.PHONY: all clean runstatic codestyle

.c.o: pager.h 
	gcc -g -c $<

clean:
	@echo "Cleaning the executables and *.o files"
	@rm -f *.o
	@rm -f apager
	@rm -f *.orig
	@rm -f apager.ammi
	@rm -f dpager
	@rm -f hpager
	cd testsuit ; make clean
	cd mallocfilter/src ; make clean

codestyle:
	@astyle --style=google apager.c
	@astyle --style=google pager.h
	@astyle --style=google stackutils.c
	@astyle --style=google hello.c
	@astyle --style=google toload.c


run:
	@make all
	cd testsuit ; make run
	
