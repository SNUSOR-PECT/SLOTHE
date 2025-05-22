#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <NAF>"
  exit 1
fi

echo -e "\n[*] Draw a CFG of a target function ($1)"

/usr/local/bin/clang -O2 -c -emit-llvm ./math/_$1.c -o ./temp/_$1.bc
/usr/local/bin/llvm-dis ./temp/_$1.bc
/usr/local/bin/opt -load-pass-plugin ./build/lib/libCountCond.so -passes=count-conditions -disable-output ./temp/_$1.ll

/usr/local/bin/opt -passes=dot-cfg -disable-output ./temp/_$1.ll
if dot -Tpng -o ./results/_$1.png ./._$1.dot; then
    echo -e "[*] A new CFG is drawn at ./results/_$1.png!"
else
    echo -e "[*] Fail to draw a new CFG :("
fi