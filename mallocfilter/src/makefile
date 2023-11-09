all:	mallocfreefilter.so mallocfreemain
	echo "shared library mallocfreefilter.so and test executable mallocfreemain is built"


.PHONY: all clean


mallocfreefilter.so:	mallocfreefilter.o
	gcc -shared -o mallocfreefilter.so mallocfreefilter.o -ldl 


mallocfreemain:	mallocfreemain.o mallocfreefilter.so
	gcc -o mallocfreemain mallocfreemain.o mallocfreefilter.so




clean:
	rm -f *.o
	rm -f *.so
	rm -f mallocfreemain

