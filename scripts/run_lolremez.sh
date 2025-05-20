#!/bin/bash

if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <source> <target> <output>"
  exit 1
fi

# e.g. bash scripts/run_lolremez.sh temp/optim.ll expm1 new_tanh

DIR=./temp/
if [ ! -d $DIR ]; then
    mkdir temp
fi

# 1. run lolremez with desired input range
# TODO : input range, degree
lolremez --double -r "-8:8" "$2(x)" -d 21 > temp/temp_$2.c

# 2. compile approximated polynomial
/usr/local/bin/clang -O2 -c -emit-llvm temp/temp_$2.c -o temp/temp_$2.bc
/usr/local/bin/llvm-dis temp/temp_$2.bc
/usr/local/bin/opt -disable-output temp/temp_$2.ll

# 3. link top function and approximated polynomial
/usr/local/bin/llvm-link $1 temp/temp_$2.ll -S -o temp/merged.ll

# 4. run ReplaceFunc pass
/usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceFunc.so -passes=replace-func -target-func=$2 -S -o temp/$3.ll temp/merged.ll