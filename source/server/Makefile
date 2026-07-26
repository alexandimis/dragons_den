CFLAGS = -Wall -g -fsanitize=address -pthread -D PRINT_MAP -D PRINT_USERS # -D DEBUG

run: server.o
	cp ../game/libhw3.* .
	gcc $(CFLAGS) server.o ../objects/random.o ../objects/util.o ../objects/graph.o -L. -lhw3 -o run
	mv *.o ../objects
	rm -rf libhw3.*

server.o: server.c server.h
	gcc $(CFLAGS) -c server.c

clean:
	rm -rf *.o run
