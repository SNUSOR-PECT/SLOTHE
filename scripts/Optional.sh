#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <input>"
  exit 1
fi

src=$1
dst=$2

# Generate T0.txt and F1.txt
/usr/local/bin/llc -filetype=obj ./temp/$1.ll -o $1.o
/usr/local/bin/clang++ scripts/optim.cpp $1.o -o optim -lm
./optim $1 $2

# Re-compile
/usr/local/bin/opt -O2 ./temp/$2.ll -S -o ./temp/$2.ll

/usr/local/bin/opt -passes=dot-cfg -disable-output ./temp/$2.ll
if dot -Tpng -o ./results/_tanh_$2.png ./._tanh.dot;then
    echo -e "\n[*] A new CFG is drawn at ./results/_tanh_$2.png!"
else
    echo -e "\n[*] Fail to draw a new CFG :("
fi

rm -rf optim $1.o