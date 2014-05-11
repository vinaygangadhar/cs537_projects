#!/bin/sh

clear
make clean
make

rm test1
gcc -lmfs  -L. -o test1 test1.c -Wall -Werror

./test1
