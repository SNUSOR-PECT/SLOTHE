#!/bin/bash

echo -e "\n[*] Draw a CFG of a target function (tanh)\n"

/usr/local/bin/clang -O2 -c -emit-llvm ./math/_tanh.c -o ./temp/_tanh.bc
/usr/local/bin/llvm-dis ./temp/_tanh.bc
/usr/local/bin/opt -disable-output ./temp/_tanh.ll
# rm -rf _tanh.bc _tanh.ll

/usr/local/bin/opt -passes=dot-cfg -disable-output ./temp/_tanh.ll
if dot -Tpng -o ./results/_tanh.png ./._tanh.dot; then
    echo -e "\n[*] A new CFG is drawn at ./results/_tanh.png!"
else
    echo -e "\n[*] Fail to draw a new CFG :("
fi