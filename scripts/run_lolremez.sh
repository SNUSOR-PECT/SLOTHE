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
# - NAF is replaced with approximated function
/usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceFunc.so -passes=replace-func,dce -target-func=$2 -S -o temp/replaced.ll temp/merged.ll

# 5. find maximum value of the input of div
# Output
# - NAF is replaced with approximated function
# - The function returns the input of div
/usr/local/bin/opt -load-pass-plugin ./build/lib/libGetDivisorRange.so -passes=get-div-range,dce -target-func=$2 -S -o temp/temp_$3.ll temp/merged.ll
/usr/local/bin/llc -filetype=obj temp/temp_$3.ll -o temp_$3.o
/usr/local/bin/clang++ ./scripts/checkMax.cpp temp_$3.o -o checkMax -lm
divMax=$(./checkMax)
echo "divMax : $divMax"

# 6. compile approximated divide (inverse)
/usr/local/bin/clang -O2 -c -emit-llvm math/_inverse.c -o temp/_inverse.bc
/usr/local/bin/llvm-dis temp/_inverse.bc
/usr/local/bin/opt -disable-output temp/_inverse.ll

# 7. link top function and approximated polynomial
/usr/local/bin/llvm-link temp/replaced.ll temp/_inverse.ll -S -o temp/merged_div.ll

# 8. run ReplaceDiv pass
/usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceDiv.so -passes=replace-div,dce -div-max=$divMax -iter-d=14 -S -o temp/result.ll temp/merged_div.ll

# 9. check validity
bash ./scripts/checkValid.sh result 4