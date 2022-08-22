all: main.o
	gcc -Ofast main.o -o chessengine
	rm -rf *.o

main.o: main.c
	gcc -Ofast -c main.c

collator.o: nbincollator.c
	gcc -Ofast -c nbincollator.c

clean:
	rm -rf *.o

