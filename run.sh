#!/bin/bash

if [ $# -le 0 ];then
    echo -e "(Error) Enter the option: 0, 1 \n./run.sh [option]"
    exit
fi

if [ $1 -eq 0 ];then
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libBasicTrace.so -passes=basic-trace -disable-output _tanh.ll
fi

if [ $1 -eq 1 ];then
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libRemoveSpecials.so -passes=remove-specials,simplifycfg _tanh.ll -o _tanh_RemoveSpecials.bc
    /usr/local/bin/llvm-dis _tanh_RemoveSpecials.bc
    rm -rf _tanh_RemoveSpecials.bc
    
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libRemoveSpecials.so -passes=dot-cfg -disable-output _tanh_RemoveSpecials.ll
    dot -Tpng -o ./assets/_tanh_RemoveSpecials.png ./._tanh.dot
    echo -e "\n[*] A new CFG is drawn at ./assets/_tanh_RemoveSpecials.png!"
fi