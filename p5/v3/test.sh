#!/bin/sh

clear
make clean
make

rm test
gcc -lmfs  -L. -o test test.c -Wall -Werror

./test
