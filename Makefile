all: main.o
	nasm -Ox -fmacho64 influenceMapForSquare.asm
	gcc -O3 main.o influenceMapForSquare.o -o chessengine
	rm -rf *.o
main.o: main.c
	gcc -O3 -c main.c
clean:
	rm -rf *.o
