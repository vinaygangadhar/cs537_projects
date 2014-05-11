#!/bin/sh

clear
make clean
make

rm test2
gcc -lmfs  -L. -o test2 test2.c -Wall -Werror

./test2
