#!/bin/sh
clear
make clean
make
clear
rm 1.img
./server 1245 1.img
