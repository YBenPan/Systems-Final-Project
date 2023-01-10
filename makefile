OBJECTS= vector.o
CFLAGS= -Wall -Wextra
LDFLAGS= -lm

vector.o: vector.c vector.h
	gcc -c $(CFLAGS) vector.c
