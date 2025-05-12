#!/bin/bash

if [ $# -le 0 ];then
    echo -e "(Error) Enter the option: 0, 1 \n./run.sh [option]"
    exit
fi

if [ $1 -eq 0 ];then
    echo -e "\n[*] Apply BasicTrace pass on tanh\n"
    /usr/local/bin/clang -c -emit-llvm ./math/_tanh.c -o _tanh.bc
    /usr/local/bin/llvm-dis _tanh.bc
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libBasicTrace.so -passes=basic-trace -disable-output _tanh.ll
    rm -rf _tanh.bc _tanh.ll
fi

if [ $1 -eq 1 ];then
    echo -e "\n[*] Apply RemoveSpecial and SimplifyCFG pass on tanh\n"
    /usr/local/bin/clang -c -emit-llvm ./math/_tanh.c -o _tanh.bc
    /usr/local/bin/llvm-dis _tanh.bc # output: _tanh.ll
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libRemoveSpecials.so -passes=remove-specials,simplifycfg _tanh.ll -o _tanh_RemoveSpecials.bc
    /usr/local/bin/llvm-dis _tanh_RemoveSpecials.bc
    
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libRemoveSpecials.so -passes=dot-cfg -disable-output _tanh_RemoveSpecials.ll
    if dot -Tpng -o ./assets/_tanh_RemoveSpecials.png ./._tanh.dot;then
        echo -e "\n[*] A new CFG is drawn at ./assets/_tanh_RemoveSpecials.png!"
    else
        echo -e "\n[*] Fail to draw a new CFG :("
        exit 1
    fi
    rm -rf _tanh*.bc _tanh*.ll
    rm -rf ./.*.dot
fi

#if [ $1 -eq 2 ];then
#    echo -e "\n[*] Apply FunctionalityCheck pass\n"
#    /usr/local/bin/opt -load-pass-plugin ./build/lib/libFunctionalityCheck.so -passes=functionality-check -disable-output _tanh.ll
#fi
