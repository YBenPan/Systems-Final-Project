OBJECTS= vector.o table.o
CFLAGS= -Wall -Wextra
LDFLAGS= -lm

server: serverprog
	./serverprog

client: clientprog
	./clientprog

serverprog: server.o
	gcc $(CFLAGS) -o serverprog server.o $(LDFLAGS)

clientprog: client.o
	gcc $(CFLAGS) -o clientprog client.o $(LDFLAGS)

server.o: server.c networking.h error_handler.h
	gcc -c $(CFLAGS) server.c

client.o: client.c networking.h error_handler.h
	gcc -c $(CFLAGS) client.c

start: main
	./main

main: main.o parser.o table.o vector.o file_io.o schema.o datatypes.o
	gcc $(CFLAGS) -o main main.o parser.o table.o vector.o file_io.o schema.o datatypes.o $(LDFLAGS)

tabledebug: tabledebug.o table.o vector.o file_io.o schema.o datatypes.o
	gcc $(CFLAGS) -o tabledebug tabledebug.o table.o vector.o file_io.o schema.o datatypes.o $(LDFLAGS)

datatypedebug: datatypedebug.o datatypes.o
	gcc $(CFLAGS) -o datatypedebug datatypedebug.o datatypes.o

main.o: main.c parser.h file_io.h vector.h table.h
	gcc -c $(CFLAGS) main.c

parser.o: parser.c parser.h table.h vector.h file_io.h
	gcc -c $(CFLAGS) parser.c

datatypedebug.o: datatypedebug.c datatypes.h
	gcc -c $(CFLAGS) datatypedebug.c

tabledebug.o: tabledebug.c table.h vector.h file_io.h schema.h datatypes.h
	gcc -c $(CFLAGS) tabledebug.c

datatypes.o: datatypes.c datatypes.h
	gcc -c $(CFLAGS) datatypes.c

file_io.o: file_io.c file_io.h table.h vector.h
	gcc -c $(CFLAGS) file_io.c

table.o: table.c table.h vector.h
	gcc -c $(CFLAGS) table.c

vector.o: vector.c vector.h
	gcc -c $(CFLAGS) vector.c

clean:
	rm -f *.o
	rm -f tabledebug
	rm -f datatypedebug
	rm -f main
