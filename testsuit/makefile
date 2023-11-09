testsuit:	staticsuits  instruments xpager
	echo "all Test suit is built"
	echo "make runapager or make rundpager" to run the tests 



staticsuits:	hello.static  toload.static sigsegv.static bigarrays.static instruments.o
	echo "staticsuits is built"



hello.static:	hello.static.o instruments.o
	gcc -static -Wl,-z,norelro -o hello.static hello.static.o instruments.o

toload.static:	toload.static.o instruments.o
	gcc -static -Wl,-z,norelro -o toload.static toload.static.o instruments.o

sigsegv.static:	sigsegv.static.o instruments.o
	gcc -static -Wl,-z,norelro -o sigsegv.static sigsegv.static.o instruments.o

bigarrays.static:	bigarrays.static.o instruments.o
	gcc -static -Wl,-z,norelro -o bigarrays.static bigarrays.static.o instruments.o

instruments:	instruments_main.o
	gcc -Wl,-z,norelro  -o instruments instruments_main.o

	




toload.static.o:	toload.c
	gcc -g -o toload.static.o -c toload.c

hello.static.o:	hello.c
	gcc -g  -c -o hello.static.o hello.c

sigsegv.static.o:	sigsegv.c
	gcc -g  -c -o sigsegv.static.o sigsegv.c

bigarrays.static.o:	bigarrays.c
	gcc -g -c -o bigarrays.static.o bigarrays.c

instruments.o:	instruments.c instruments.h
	gcc -g -c -o instruments.o instruments.c

xpager:		xpager.o
	gcc -o xpager xpager.o

xpager.o:	xpager.c
	gcc -c -o xpager.o xpager.c

instruments_main.o:	instruments.c instruments.h
	gcc -g -c -DINSTRUMENT_MAIN -o instruments_main.o instruments.c

.PHONY:	all clean runapager rundpager run
.c.o:	
	gcc -g  -c -o $^.static.o  $^.c


runapager:
	@../apager ./hello.static hello
	@../apager ./toload.static hello static
	@../apager ./bigarrays.static 5000 -h
	@../apager ./bigarrays.static 500
	@../apager ./bigarrays.static 50
	@../apager ./bigarrays.static 5


rundpager:
	@../dpager ./hello.static hello
	@../dpager ./toload.static hello static
	@../dpager ./bigarrays.static 5000 -h
	@../dpager ./bigarrays.static 500
	@../dpager ./bigarrays.static 50
	@../dpager ./bigarrays.static 5

runhpager:
	@../hpager ./hello.static hello
	@../hpager ./toload.static hello static
	@../hpager ./bigarrays.static 5000 -h
	@../hpager ./bigarrays.static 500 
	@../hpager ./bigarrays.static 50
	@../hpager ./bigarrays.static 5

runxpager:
	@./xpager ./hello.static hello
	@./xpager ./toload.static hello static
	@./xpager ./bigarrays.static 5000 -h
	@./xpager ./bigarrays.static 500
	@./xpager ./bigarrays.static 50
	@./xpager ./bigarrays.static 5


clean:
	rm -f *.o
	rm -f *.static
	rm -f instruments
	rm -f xpager




run:
	echo "" > results.log
	echo "DPAGER" >> results.log
	make rundpager
	echo "APAGER" >> results.log
	make runapager
	echo "HPAGER" >> results.log
	make runhpager
	echo "XPAGER" >> results.log
	make runxpager
