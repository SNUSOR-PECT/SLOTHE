#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <input> <output>"
  exit 1
fi

# Apply Mergeable and SimplifyCFG pass
echo -e "\n[*] Apply Mergeable and SimplifyCFG pass on tanh\n"
/usr/local/bin/opt -load-pass-plugin ./build/lib/libMergeable.so -passes=find-mergeable,simplifycfg ./temp/$1.ll -o ./temp/$1_swapped.bc
/usr/local/bin/llvm-dis ./temp/$1_swapped.bc

/usr/local/bin/opt -load-pass-plugin ./build/lib/libMergeable.so -passes=dot-cfg -disable-output ./temp/$1_swapped.ll
# if dot -Tpng -o ./results/$1_swapped.png ./._tanh.dot;then
#     echo -e "\n[*] A new CFG is drawn at ./results/$1_swapped.png!"
# else
#     echo -e "\n[*] Fail to draw a new CFG :("
# fi

# Generate T0.txt and F1.txt
/usr/local/bin/llc -filetype=obj ./temp/$1.ll -o $1.o
/usr/local/bin/clang++ ./scripts/checkMerge.cpp $1.o -o checkMerge -lm
./checkMerge T 0 $1
./checkMerge F 1 $1

# Generate T1.txt and F0.txt
/usr/local/bin/llc -filetype=obj ./temp/$1_swapped.ll -o $1_swapped.o
/usr/local/bin/clang++ ./scripts/checkMerge.cpp $1_swapped.o -o checkMerge -lm
./checkMerge T 1 $1
./checkMerge F 0 $1

# Check mergeability
/usr/local/bin/clang++ ./scripts/isMergeable.cpp -o isMergeable -lm
isMergeable=$(./isMergeable 6)
# echo "isMergeable? : $isMergeable"

if [ "$isMergeable" -eq 0 ] || [ "$isMergeable" -eq 1 ];then
    echo -e "\n[*] Apply Merge and SimplifyCFG pass on tanh\n"
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libMerge.so -passes=merge-path,simplifycfg -merge-direction=$isMergeable ./temp/$1.ll -o ./temp/$2.bc
    /usr/local/bin/llvm-dis ./temp/$2.bc
    
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libMerge.so -passes=dot-cfg -disable-output ./temp/$2.ll
    if dot -Tpng -o ./results/$2.png ./._tanh.dot;then
        echo -e "\n[*] A new CFG is drawn at ./results/$2.png!"
    else
        echo -e "\n[*] Fail to draw a new CFG :("
    fi
fi

rm -rf *.o checkMerge isMergeable