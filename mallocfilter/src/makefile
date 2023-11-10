all:	mallocfreefilter.so mallocfreemain mallocfilter2.so malloctest 
	@echo "shared library mallocfreefilter.so and test executable mallocfreemain are built"


.PHONY: all clean runmalloctest codingstandard

codingstandard:
	@astyle --style=linux mallocfreemain.c
	@astyle --style=linux mallocfreefilter.c
	@astyle --style=linux malloctest.c
	@astyle --style=linux mallocfilter2.c


mallocfreefilter.so:	mallocfreefilter.c
	gcc -shared -o mallocfreefilter.so mallocfreefilter.c -fPIC -ldl 


mallocfreemain:	mallocfreemain.o mallocfreefilter.so
	gcc -o mallocfreemain  mallocfreemain.o

mallocfreemain.o:	mallocfreemain.c
	gcc -c -g -o mallocfreemain.o mallocfreemain.c





mallocfreefilterx.o:	mallocfreefilter.c
	gcc -c -g -o mallocfreefilterx.o mallocfreemain.c





mallocfilter2.so:	mallocfilter2.c
	gcc mallocfilter2.c -shared -o mallocfilter2.so -fPIC -ldl


malloctest:	malloctest.o
	gcc -o malloctest malloctest.o

malloctest.o:	malloctest.c
	gcc -c -o malloctest.o malloctest.c

runmalloctest:
	make malloctest
	@./malloctest
	@LD_PRELOAD=`pwd`/mallocfilter2.so ./malloctest

runfilter:
	make mallocfreemain
	@./mallocfreemain
	@LD_PRELOAD=`pwd`/mallocfreefilter.so ./mallocfreemain



clean:
	@echo "Cleaning the mallocfreemain directory"
	@rm -f *.o
	@rm -f *.so
	@rm -f mallocfreemain
	@rm -f malloctest
	@rm -f *.orig

