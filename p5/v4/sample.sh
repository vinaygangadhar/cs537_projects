#!/bin/sh

for i in `seq 1 64`;
do
	echo "creating $i.txt"
	./mfsput $i.txt 2.img / 
done
