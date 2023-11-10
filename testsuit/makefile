testsuit:	staticsuits  instruments xpager
	@echo "all Test suit is built"
	@echo "'make run' to run the doe"



staticsuits:	hello.static  toload.static sigsegv.static bigarrays.static instruments.o seqarray.static malloctest.static
	@echo "staticsuits is built"



hello.static:	hello.static.o instruments.o
	gcc -static -Wl,-z,norelro -o hello.static hello.static.o instruments.o

toload.static:	toload.static.o instruments.o
	gcc -static -Wl,-z,norelro -o toload.static toload.static.o instruments.o

sigsegv.static:	sigsegv.static.o instruments.o
	gcc -static -Wl,-z,norelro -o sigsegv.static sigsegv.static.o instruments.o

bigarrays.static:	bigarrays.static.o instruments.o
	gcc -static -Wl,-z,norelro -o bigarrays.static bigarrays.static.o instruments.o


seqarray.static:	seqarray.static.o instruments.o
	gcc -static -Wl,-z,norelro -o seqarray.static seqarray.static.o instruments.o

malloctest.static:	malloctest.static.o instruments.o
	gcc -static -Wl,-z,norelro -o malloctest.static malloctest.static.o instruments.o


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

seqarray.static.o:	seqarray.c
	gcc -g -c -o seqarray.static.o seqarray.c

malloctest.static.o:	malloctest.c
	gcc -g -c -o malloctest.static.o malloctest.c



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
	@../apager ./bigarrays.static 5
	@../apager ./bigarrays.static 5
	@../apager ./bigarrays.static 5
	@../apager ./bigarrays.static 5
	@../apager ./bigarrays.static 5
	@../apager ./seqarray.static 5 -h
	@../apager ./seqarray.static 5 
	@../apager ./seqarray.static 5 
	@../apager ./seqarray.static 5 
	@../apager ./seqarray.static 5 
	@../apager ./seqarray.static 5 


rundpager:
	@../dpager ./hello.static hello
	@../dpager ./toload.static hello static
	@../dpager ./bigarrays.static 5000 -h
	@../dpager ./bigarrays.static 500
	@../dpager ./bigarrays.static 50
	@../dpager ./bigarrays.static 5
	@../dpager ./bigarrays.static 5
	@../dpager ./bigarrays.static 5
	@../dpager ./bigarrays.static 5
	@../dpager ./bigarrays.static 5
	@../dpager ./bigarrays.static 5
	@../dpager ./seqarray.static 5 -h
	@../dpager ./seqarray.static 5 
	@../dpager ./seqarray.static 5 
	@../dpager ./seqarray.static 5 
	@../dpager ./seqarray.static 5 
	@../dpager ./seqarray.static 5 

runhpager:
	@../hpager ./hello.static hello
	@../hpager ./toload.static hello static
	@../hpager ./bigarrays.static 5000 -h
	@../hpager ./bigarrays.static 500 
	@../hpager ./bigarrays.static 50
	@../hpager ./bigarrays.static 5
	@../hpager ./bigarrays.static 5
	@../hpager ./bigarrays.static 5
	@../hpager ./bigarrays.static 5
	@../hpager ./bigarrays.static 5
	@../hpager ./bigarrays.static 5
	@../hpager ./seqarray.static 5 -h
	@../hpager ./seqarray.static 5 
	@../hpager ./seqarray.static 5 
	@../hpager ./seqarray.static 5 
	@../hpager ./seqarray.static 5 
	@../hpager ./seqarray.static 5 

runh3pager:
	@../h3pager ./hello.static hello
	@../h3pager ./toload.static hello static
	@../h3pager ./bigarrays.static 5000 -h
	@../h3pager ./bigarrays.static 500 
	@../h3pager ./bigarrays.static 50
	@../h3pager ./bigarrays.static 5
	@../h3pager ./bigarrays.static 5
	@../h3pager ./bigarrays.static 5
	@../h3pager ./bigarrays.static 5
	@../h3pager ./bigarrays.static 5
	@../h3pager ./bigarrays.static 5
	@../h3pager ./seqarray.static 5 -h
	@../h3pager ./seqarray.static 5 
	@../h3pager ./seqarray.static 5 
	@../h3pager ./seqarray.static 5 
	@../h3pager ./seqarray.static 5 
	@../h3pager ./seqarray.static 5 

runxpager:
	@./xpager ./hello.static hello
	@./xpager ./toload.static hello static
	@./xpager ./bigarrays.static 5000 -h
	@./xpager ./bigarrays.static 500
	@./xpager ./bigarrays.static 50
	@./xpager ./bigarrays.static 5
	@./xpager ./bigarrays.static 5
	@./xpager ./bigarrays.static 5
	@./xpager ./bigarrays.static 5
	@./xpager ./bigarrays.static 5
	@./xpager ./bigarrays.static 5
	@./xpager ./seqarray.static 5 -h
	@./xpager ./seqarray.static 5 
	@./xpager ./seqarray.static 5 
	@./xpager ./seqarray.static 5 
	@./xpager ./seqarray.static 5 
	@./xpager ./seqarray.static 5 


runbigdata:
	@echo "BIGARRAY BSS but LARGE ACCESS" >> results.log
	@echo "APAGER" >> results.log
	@../apager ./bigarrays.static 5 -h
	@echo "DPAGER" >> results.log
	@../dpager ./bigarrays.static 5
	@echo "HPAGER" >> results.log
	@../hpager ./bigarrays.static 5
	@echo "3PAGER" >> results.log
	@../h3pager ./bigarrays.static 5
	@echo "XPAGER" >> results.log
	@./xpager ./bigarrays.static 5

runsmalldata:
	@echo "BIG ARRAY BSS but SMALL access" >> results.log
	@echo "APAGER" >> results.log
	@../apager ./bigarrays.static 5000 -h
	@echo "DPAGER" >> results.log
	@../dpager ./bigarrays.static 5000
	@echo "HPAGER" >> results.log
	@../hpager ./bigarrays.static 5000
	@echo "3PAGER" >> results.log
	@../h3pager ./bigarrays.static 5000
	@echo "XPAGER" >> results.log
	@./xpager ./bigarrays.static 5000

runseq:
	@echo "SEQUENTIAL ARRAY" >> results.log
	@echo "APAGER" >> results.log
	@../apager ./seqarray.static 5000 -h
	@echo "DPAGER" >> results.log
	@../dpager ./seqarray.static 5000
	@echo "HPAGER" >> results.log
	@../hpager ./seqarray.static 5000
	@echo "3PAGER" >> results.log
	@../h3pager ./seqarray.static 5000
	@echo "XPAGER" >> results.log
	@./xpager ./seqarray.static 5000

runcode:
	@echo "small data and small code " >> results.log
	@echo "APAGER" >> results.log
	@../apager ./toload.static 5000 -h
	@echo "DPAGER" >> results.log
	@../dpager ./toload.static 5000 -h
	@echo "HPAGER" >> results.log
	@../hpager ./toload.static 5000 -h
	@echo "3PAGER" >> results.log
	@../h3pager ./toload.static 5000 -h
	@echo "XPAGER" >> results.log
	@./xpager ./toload.static 5000 -h


clean:
	echo "Cleaning testsuit directory"
	@rm -f *.o
	@rm -f *.static
	@rm -f instruments
	@rm -f xpager




run:
	@echo "" > results.log
	@echo "APAGER" >> results.log
	@make runapager
	@echo "" >> results.log
	@echo "DPAGER" >> results.log
	@make rundpager
	@echo "" >> results.log
	@echo "HPAGER" >> results.log
	@make runhpager
	@echo "" >> results.log
	@echo "H3PAGER" >> results.log
	@make runh3pager
	@echo "" >> results.log
	@echo "XPAGER" >> results.log
	@make runxpager
	@echo "" >> results.log

	@cp results.log results`date +"-%Y%m%d-%H%M"`.log

runmore:
	make runbigdata 
	make runsmalldata
	make runseq
	make runcode
	@cp results.log resultsmore`date +"-%Y%m%d-%H%M"`.log
