all: main.o
	gcc -O3 main.o -o chessengine
	rm -rf *.o
main.o: main.c
	gcc -O3 -c main.c
clean:
	rm -rf *.o
