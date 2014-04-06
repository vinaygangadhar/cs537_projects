#!/bin/sh
clear
make clean
make
gcc -lmem -L. -o myprogram mymain.c -Wall -Werror
./myprogram
