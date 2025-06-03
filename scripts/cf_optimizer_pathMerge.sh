#!/bin/bash

if [ "$#" -ne 4 ]; then
  echo "Usage: $0 <NAF> <input> <output> <precision>"
  exit 1
fi

# Apply Mergeable and SimplifyCFG pass
echo -e "[*] Apply Swap and SimplifyCFG pass on $1"
/usr/bin/opt-16 -load-pass-plugin ./build/lib/libSwap.so -passes=swap-path,simplifycfg ./temp/$2.ll -o ./temp/$2_swapped.bc
/usr/bin/llvm-dis-16 ./temp/$2_swapped.bc

# Generate T0.txt and F1.txt
/usr/bin/llc-16 -filetype=obj ./temp/$2.ll -o $2.o
/usr/bin/clang++-16 ./utils/checkMerge.cpp $2.o -o checkMerge -lm
./checkMerge T 0 $2
./checkMerge F 1 $2

# Generate T1.txt and F0.txt
/usr/bin/llc-16 -filetype=obj ./temp/$2_swapped.ll -o $2_swapped.o
/usr/bin/clang++-16 ./utils/checkMerge.cpp $2_swapped.o -o checkMerge -lm
./checkMerge T 1 $2
./checkMerge F 0 $2

# Check mergeability
/usr/bin/clang++-16 ./utils/isMergeable.cpp -o isMergeable -lm
isMergeable=$(./isMergeable $4)
# echo "isMergeable? : $isMergeable"

if [ "$isMergeable" -eq 0 ] || [ "$isMergeable" -eq 1 ];then
    echo -e "[*] Apply Swap and SimplifyCFG pass on $1"
    /usr/bin/opt-16 -load-pass-plugin ./build/lib/libMerge.so -passes=merge-path,simplifycfg -merge-direction=$isMergeable ./temp/$2.ll -o ./temp/$3.bc
    /usr/bin/llvm-dis-16 ./temp/$3.bc
    
    /usr/bin/opt-16 -load-pass-plugin ./build/lib/libMerge.so -passes=dot-cfg -disable-output ./temp/$3.ll
    /usr/bin/dot -Tpng -o ./results/$3.png ./._$1.dot
    # if /usr/bin/dot -Tpng -o ./results/$3.png ./._$1.dot;then
    #     echo -e "[*] A new CFG is drawn at ./results/$3.png!"
    # else
    #     echo -e "[*] Fail to draw a new CFG :("
    # fi
fi

rm -rf *.o checkMerge isMergeable