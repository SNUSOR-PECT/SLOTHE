#!/bin/bash

if [ $# -le 0 ];then
    echo -e "(Error) Enter the option: 0, 1, 2 \n./run.sh [option]"
    exit
fi

if [ $1 -eq 0 ];then
    echo -e "\n[*] Apply BasicTrace pass on tanh\n"
    /usr/local/bin/clang -O2 -c -emit-llvm ./math/_tanh.c -o _tanh.bc
    /usr/local/bin/llvm-dis _tanh.bc
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libBasicTrace.so -passes=basic-trace -disable-output _tanh.ll
    rm -rf _tanh.bc _tanh.ll
fi

if [ $1 -eq 1 ];then
    echo -e "\n[*] Apply RemoveSpecial and SimplifyCFG pass on tanh\n"
    /usr/local/bin/clang -O2 -c -emit-llvm ./math/_tanh.c -o _tanh.bc
    /usr/local/bin/llvm-dis _tanh.bc # output: _tanh.ll
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libUnreachablePath.so -passes=remove-unreachable,simplifycfg _tanh.ll -o _tanh_UnreachablePath.bc
    /usr/local/bin/llvm-dis _tanh_UnreachablePath.bc
    
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libUnreachablePath.so -passes=dot-cfg -disable-output _tanh_UnreachablePath.ll
    if dot -Tpng -o ./assets/_tanh_UnreachablePath.png ./._tanh.dot;then
        echo -e "\n[*] A new CFG is drawn at ./assets/_tanh_UnreachablePath.png!"
    else
        echo -e "\n[*] Fail to draw a new CFG :("
    fi
    # rm -rf _tanh*.bc _tanh*.ll
    rm -rf ./.*.dot
fi

if [ $1 -eq 2 ];then
    echo -e "\n[*] Apply Mergeable and SimplifyCFG pass on tanh after RemoveSpecial\n"
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libMergeable.so -passes=find-mergeable,simplifycfg _tanh_UnreachablePath.ll -o _tanh_Mergeable_swapped.bc
    /usr/local/bin/llvm-dis _tanh_Mergeable_swapped.bc
    
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libMergeable.so -passes=dot-cfg -disable-output _tanh_Mergeable_swapped.ll
    if dot -Tpng -o ./assets/_tanh_Mergeable_swapped.png ./._tanh.dot;then
        echo -e "\n[*] A new CFG is drawn at ./assets/_tanh_Mergeable_swapped.png!"
    else
        echo -e "\n[*] Fail to draw a new CFG :("
    fi
    # rm -rf _tanh*.bc _tanh*.ll
    # rm -rf ./.*.dot
fi

if [ $1 -eq 9 ];then
    rm -rf *.bc *.ll *.o ./.*.dot
    rm -rf T0.txt T1.txt F0.txt F1.txt
fi
