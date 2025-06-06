#!/bin/bash

if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <NAF> <input> <output>"
  exit 1
fi

# Generate T0.txt and F1.txt
/usr/bin/llc-16 -filetype=obj ./temp/$2.ll -o $2.o
/usr/bin/clang++-16 utils/optim.cpp $2.o -o optim -lm
./optim $2 $3

# Re-compile
/usr/bin/opt-16 -O2 ./temp/$3.ll -S -o ./temp/$3.ll
/usr/bin/opt-16 -passes=dot-cfg -disable-output ./temp/$3.ll
/usr/bin/dot -Tpng -o ./results/_$3.png ./._$1.dot
# if /usr/bin/dot -Tpng -o ./results/_$3.png ./._$1.dot;then
#     echo -e "\n[*] A new CFG is drawn at ./results/_$3.png!"
# else
#     echo -e "\n[*] Fail to draw a new CFG :("
# fi

rm -rf optim $2.o