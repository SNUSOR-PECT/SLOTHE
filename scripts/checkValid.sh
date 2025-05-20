#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <input> <precision>"
  exit 1
fi

# Generate T0.txt and F1.txt
/usr/local/bin/llc -filetype=obj ./temp/$1.ll -o $1.o
/usr/local/bin/clang++ scripts/checkPrec.cpp $1.o -o checkPrec -lm
./checkPrec $2

rm -rf checkPrec $1.o