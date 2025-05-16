#!/bin/bash

echo -e "\n[*] Apply UnreachablePath and SimplifyCFG pass on tanh\n"

/usr/local/bin/opt -load-pass-plugin ./build/lib/libUnreachable.so -passes=remove-unreachable,simplifycfg ./temp/_tanh.ll -o ./temp/_tanh_Unreachable.bc
/usr/local/bin/llvm-dis ./temp/_tanh_Unreachable.bc

/usr/local/bin/opt -load-pass-plugin ./build/lib/libUnreachable.so -passes=dot-cfg -disable-output ./temp/_tanh_Unreachable.ll
if dot -Tpng -o ./results/_tanh_Unreachable.png ./._tanh.dot;then
    echo -e "\n[*] A new CFG is drawn at ./results/_tanh_Unreachable.png!"
else
    echo -e "\n[*] Fail to draw a new CFG :("
fi