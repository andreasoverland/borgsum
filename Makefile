all: main.o nbincollator.o
	gcc -Ofast mainnr.o -o chessengine
	gcc -Ofast nbincollator.o -o nbincollator
	rm -rf *.o
main.o: mainnr.c
	gcc -Ofast -c mainnr.c

nbincollator.o: nbincollator.c
	gcc -Ofast -c nbincollator.c



clean:
	rm -rf *.o

