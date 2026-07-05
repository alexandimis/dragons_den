CFLAGS = -Wall -g -fsanitize=address -D DEBUG

play: fileio.o game.o graph.o hw3.o random.o util.o
	gcc $(CFLAGS) fileio.o game.o graph.o hw3.o random.o util.o -L. -lhw3 -o play
	rm -rf *.o
	mv play ../bin

fileio.o: fileio.c fileio.h
	gcc $(CFLAGS) -c fileio.c

game.o: game.c game.h
	gcc $(CFLAGS) -c game.c

graph.o: graph.c graph.h
	gcc $(CFLAGS) -c graph.c

hw3.o: hw3.c hw3.h
	gcc $(CFLAGS) -c hw3.c

random.o: random.c random.h
	gcc $(CFLAGS) -c random.c

util.o: util.c util.h
	gcc $(CFLAGS) -c util.c

clean:
	rm -rf *.o play
