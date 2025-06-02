#!/bin/bash

if [ "$#" -ne 5 ]; then
  echo "Usage: $0 <NAF> <input> <precision> <min> <max>"
  exit 1
fi

# Generate T0.txt and F1.txt
/usr/bin/llc-16 -filetype=obj ./temp/$2.ll -o $2.o
/usr/bin/clang++-16 utils/checkPrec.cpp $2.o -o checkPrec -lm
./checkPrec _$1 $3 $4 $5

rm -rf checkPrec $2.o