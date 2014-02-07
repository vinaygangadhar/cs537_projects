CC = gcc
CFLAGS = -O
CFLAGS += -Wall

all: fastsort

fastsort: fastsort.c sort.h 
	gcc $(CFLAGS) -o fastsort fastsort.c

clean:
	rm -rf fastsort
