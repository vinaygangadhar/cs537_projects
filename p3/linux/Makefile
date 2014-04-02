CC = gcc

CFLAGS= -Werror -Wall -lm

all: p3.c main2.o
	$(CC) $(CFLAGS) -o main p3.c

main2.o: mem.o helper.o
	$(CC) $(CFLAGS) -o main2.o helper.o mem.o

mem.o: mem.c helper.o
	$(CC) $(CFLAGS) -o mem.o  mem.c

helper.o: helper.c
	$(CC) $(CFLAGS) -o helper.o helper.c

clean:
	rm -f *.o
