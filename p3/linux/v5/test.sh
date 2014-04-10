#!/bin/sh
clear
make clean
make
gcc -lmem -L. -o myprogram random.c -Wall -Werror
./myprogram 0 1000 1
