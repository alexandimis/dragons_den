CFLAGS = -Wall -g -fsanitize=address -pthread # -D DEBUG

run: server.o
	gcc $(CFLAGS) server.o -o run
	rm -rf *.o

server.o: server.c server.h
	gcc $(CFLAGS) -c server.c

clean:
	rm -rf *.o run
