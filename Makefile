all: main.o nbincollator.o
	gcc -Ofast mainnr.o -o chessengine
	gcc  nbincollator.o -o nbincollator
	rm -rf *.o
main.o: mainnr.c
	gcc -Ofast -c mainnr.c

nbincollator.o: nbincollator.c
	gcc  -c nbincollator.c



clean:
	rm -rf *.o

