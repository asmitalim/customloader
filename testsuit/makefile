testsuit:	staticsuits
	echo "all Test suit is built"
	echo "make runapager or make rundpager" to run the tests 



staticsuits:	hello.static  toload.static sigsegv.static bigarrays.static
	echo "staticsuits is built"



hello.static:	hello.static.o
	gcc -static -Wl,-z,norelro -o hello.static hello.static.o

toload.static:	toload.static.o
	gcc -static -Wl,-z,norelro -o toload.static toload.static.o

sigsegv.static:	sigsegv.static.o
	gcc -static -Wl,-z,norelro -o sigsegv.static sigsegv.static.o

bigarrays.static:	bigarrays.static.o
	gcc -static -Wl,-z,norelro -o bigarrays.static bigarrays.static.o





toload.static.o:	toload.c
	gcc -g -o toload.static.o -c toload.c

hello.static.o:	hello.c
	gcc -g  -c -o hello.static.o hello.c

sigsegv.static.o:	sigsegv.c
	gcc -g  -c -o sigsegv.static.o sigsegv.c

bigarrays.static.o:	bigarrays.c
	gcc -g -c -o bigarrays.static.o bigarrays.c



.PHONY:	all clean runapager rundpager run
.c.o:	
	gcc -g  -c -o $^.static.o  $^.c


runapager:
	@../apager ./hello.static hello
	@../apager ./toload.static hello static
	@../apager ./sigsegv.static hello static
	@../apager ./bigarrays.static hello static


rundpager:
	@../dpager ./hello.static hello
	@../dpager ./toload.static hello static
	@../dpager ./sigsegv.static hello static
	@../dpager ./bigarrays.static hello static



clean:
	rm -f *.o
	rm -f *.static




run:
	make rundpager
	make runapager
