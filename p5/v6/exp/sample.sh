#!/bin/sh

for i in {1..62};
do
	./mfsput $i.txt 2.img /$i.txt	
done
