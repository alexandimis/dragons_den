CFLAGS = -Wall -g -fsanitize=address -pthread # -D DEBUG

run: server.o
	cp ../game/libhw3.* .
	gcc $(CFLAGS) server.o -L. -lhw3 -o run
	rm -rf *.o
	rm -rf libhw3.*

server.o: server.c server.h
	gcc $(CFLAGS) -c server.c

clean:
	rm -rf *.o run
