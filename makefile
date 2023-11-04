all: toload apager hello apager.ammi 
	@echo "apager Built"

toload: toload.o
	gcc -static -o toload toload.o

hello:	hello.o
	gcc -static -o hello hello.o

apager.ammi: apager.o stackutils.o pager.h
	@#gcc -static -Wl,-Ttext=0x8090000,--verbose -o apager apager.o
	@#gcc -static -Wl,--verbose -o apager apager.o
	gcc -static -Wl,--script=customlinkerfile -o apager.ammi apager.o stackutils.o

apager: apager.o stackutils.o pager.h
	@#gcc -static -Wl,-Ttext=0x8090000,--verbose -o apager apager.o
	@#gcc -static -Wl,--verbose -o apager apager.o
	gcc -static -Wl,--script=buntzlinkerfile -o apager apager.o stackutils.o


.PHONY: all clean runstatic codestyle

.c.o: pager.h 
	gcc -g -c $<

clean:
	rm -f toload
	rm -f *.o
	rm -f apager
	rm -f *.orig
	rm -f hello
	rm -f apager.ammi

runstatic:
	./apager hello

codestyle:
	@astyle --style=google apager.c
	@astyle --style=google pager.h
	@astyle --style=google stackutils.c
	@astyle --style=google hello.c
	@astyle --style=google toload.c
