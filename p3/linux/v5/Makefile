CC = gcc

CFLAGS=  -Wall 

all: mem.o helper.o 
	$(CC) -shared -o libmem.so mem.o helper.o -lm  

mem.o: mem.c
	$(CC) $(CFLAGS) -c mem.c -fpic

helper.o: helper.c
	$(CC) $(CFLAGS) -c helper.c -fpic

clean:
	rm -f *.o *.so
