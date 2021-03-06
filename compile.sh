#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ]
then
	echo "Syntax: bash compile.sh <input binary> <memory mapper bitcode file>"
	exit
fi

if [ -e "./rrpc" ]
then
	./rrpc $1
	llvm-link rrpout.bc $2 -o=rrpout-linked.bc
	llc -filetype=obj rrpout-linked.bc
	ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 /lib/crt1.o /lib/crti.o /lib/crtn.o -lc rrpout-linked.o -o test
	# rm rrpout-linked.o
	# rm rrpout-linked.bc
	# rm rrpout.bc
else
	echo "Error: RRP not found. Maybe you need to compile?"
fi
