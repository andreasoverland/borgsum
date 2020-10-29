all: main.o
	gcc -O3 mainnr.o -o chessengine
	rm -rf *.o
main.o: mainnr.c
	gcc -O3 -c mainnr.c
clean:
	rm -rf *.o
