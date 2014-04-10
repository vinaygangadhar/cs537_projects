CC = gcc

CFLAGS= -Werror  -Wall 

all: mem.o helper.o 
	$(CC) -shared -o libmem.so mem.o helper.o -lm  

mem.o: mem.c mem.h
	$(CC) $(CFLAGS) -c mem.c -fpic

helper.o: helper.c helper.h
	$(CC) $(CFLAGS) -c helper.c -fpic

clean:
	rm -f *.o *.so
