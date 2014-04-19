#!/bin/bash
killall server
make clean
clear
make
./server $1 10 10 FIFO &
echo "./client $2 $1 a &"
