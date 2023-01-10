OBJECTS= vector.o table.o
CFLAGS= -Wall -Wextra
LDFLAGS= -lm

tabledebug: tabledebug.o table.o vector.o
	gcc $(CFLAGS) -o tabledebug tabledebug.o table.o vector.o $(LDFLAGS)

tabledebug.o: tabledebug.c table.h vector.h
	gcc -c $(CFLAGS) tabledebug.c

table.o: table.c table.h vector.h
	gcc -c $(CFLAGS) table.c

vector.o: vector.c vector.h
	gcc -c $(CFLAGS) vector.c
