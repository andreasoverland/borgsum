all: main.o
	gcc -Ofast main.o -o chessengine
	rm -rf *.o
main.o: main.c
	gcc -Ofast -c main.c

clean:
	rm -rf *.o

