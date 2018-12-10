all: main.o
	nasm -fmacho64 influenceMapForSquare.asm
	gcc main.c influenceMapForSquare.o -o chessengine
	rm -rf *.o
main.o: main.c
	gcc -c main.c
clean:
	rm -rf *.o
