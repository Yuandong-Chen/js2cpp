#!/bin/bash
##make
if [ ! -f ./js2cpp ]; then
	make
fi
if [ ! -f ./$1.out ]; then
	./js2cpp < $1 > ./$1.cpp
	g++ -std=c++11 ./$1.cpp -o $1.out
	echo ----------- END OF DEBUG INFO -----------
fi
./$1.out
