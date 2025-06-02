#!/bin/bash

if [ "$#" -ne 4 ]; then
  echo "Usage: $0 <NAF> <input> <min> <max>"
  exit 1
fi

# Generate T0.txt and F1.txt
/usr/bin/llc-16 -filetype=obj ./temp/$2.ll -o $2.o
/usr/bin/clang++-16 utils/checkErr.cpp $2.o -o checkErr -lm
./checkErr _$1 $3 $4

rm -rf checkErr $2.o