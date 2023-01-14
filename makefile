OBJECTS= vector.o table.o
CFLAGS= -Wall -Wextra
LDFLAGS= -lm

tabledebug: tabledebug.o table.o vector.o file_io.o
	gcc $(CFLAGS) -o tabledebug tabledebug.o table.o vector.o file_io.o $(LDFLAGS)

parserdebug: parserdebug.o parser.o table.o vector.o file_io.o
	gcc $(CFLAGS) -o parserdebug parserdebug.o parser.o table.o vector.o file_io.o $(LDFLAGS)

tabledebug.o: tabledebug.c table.h vector.h file_io.h
	gcc -c $(CFLAGS) tabledebug.c

file_io.o: file_io.c file_io.h table.h vector.h
	gcc -c $(CFLAGS) file_io.c

table.o: table.c table.h vector.h
	gcc -c $(CFLAGS) table.c

vector.o: vector.c vector.h
	gcc -c $(CFLAGS) vector.c

clean:
	rm -f *.o
	rm -f tabledebug
	rm -f parserdebug
