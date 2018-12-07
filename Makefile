all: main.o
	nasm -fmacho64 influenceMapForSquare.asm
	gcc -O3 main.c influenceMapForSquare.o -o chessengine
	rm -rf *.o
main.o: main.c
	gcc -O3 -c main.c
clean:
	rm -rf *.o
