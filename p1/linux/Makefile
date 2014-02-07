//Makefile for p1 linux

all:


CC = gcc
C_FLAGS = -O -Wall
OBJ = fastsort
SRC = fastsort.c

fastsort: fastsort.c sort.h	

clean:
	rm -rf fastsort


hw: hw.o helper.o
	gcc -o hw hw.o helper.o -lm
hw.o: hw.c
	gcc -O -Wall -c hw.c
helper.o: helper.c
	gcc -O -Wall -c helper.c
clean:
	rm -f hw.o helper.o hw

