CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-variable -Wno-unused-result -Wno-maybe-uninitialized

all: mini-assembleur

mini-assembleur: main.o lib.o format.o
	$(CC) $(CFLAGS) main.o lib.o format.o -o mini-assembleur

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

lib.o: lib.c
	$(CC) $(CFLAGS) -c lib.c -o lib.o

format.o: format.c
	$(CC) $(CFLAGS) -c format.c -o format.o

clean:
	rm -f *.o mini-assembleur

run: mini-assembleur
	./mini-assembleur
