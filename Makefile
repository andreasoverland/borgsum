all:
	gcc -Ofast -c main.c
	gcc -Ofast -c count-duplicates-in-cbinm.c
	gcc -Ofast main.o -o chessengine
	gcc -Ofast count-duplicates-in-cbinm.o -o count-duplicates-in-cbinm
	gcc -c remdups.c
	gcc remdups.o -o remdups
	rm -rf *.o

main.o: main.c
	gcc -Ofast -c main.c

count-duplicates-in-cbinm.o: count-duplicates-in-cbinm.c
	gcc -Ofast -c count-duplicates-in-cbinm.c

collator.o: nbincollator.c
	gcc -Ofast -c nbincollator.c

remdups.o: remdups.c
	gcc -c remdups.c

clean:
	rm -rf *.o

