#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <input>"
  exit 1
fi

src=$1

# Generate T0.txt and F1.txt
/usr/local/bin/llc -filetype=obj ./temp/$1.ll -o $1.o
/usr/local/bin/clang++ scripts/checkPrec.cpp $1.o -o checkPrec -lm
./checkPrec 6

rm -rf checkPrec $1.o