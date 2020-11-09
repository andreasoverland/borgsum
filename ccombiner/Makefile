all: ccombiner.o
	gcc -O3 ccombiner.o -o ccombiner
	rm -rf *.o
ccombiner.o: ccombiner.c
	gcc -O3 -c ccombiner.c
clean:
	rm -rf *.o
