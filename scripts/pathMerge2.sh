#!/bin/bash

# Apply Mergeable and SimplifyCFG pass
echo -e "\n[*] Apply Mergeable and SimplifyCFG pass on tanh\n"
/usr/local/bin/opt -load-pass-plugin ./build/lib/libMergeable.so -passes=find-mergeable,simplifycfg _tanh_Merged.ll -o _tanh_Merged_swapped.bc
/usr/local/bin/llvm-dis _tanh_Merged_swapped.bc

/usr/local/bin/opt -load-pass-plugin ./build/lib/libMergeable.so -passes=dot-cfg -disable-output _tanh_Merged_swapped.ll
if dot -Tpng -o ./assets/_tanh_Merged_swapped.png ./._tanh.dot;then
    echo -e "\n[*] A new CFG is drawn at ./assets/_tanh_Merged_swapped.png!"
else
    echo -e "\n[*] Fail to draw a new CFG :("
fi

# Generate T0.txt and F1.txt
/usr/local/bin/llc -filetype=obj _tanh_Merged.ll -o _tanh_Merged.o
/usr/local/bin/clang++ runFunc.cpp _tanh_Merged.o -o runFunc -lm
./runFunc T 0 _tanh_Merged
./runFunc F 1 _tanh_Merged

# Generate T1.txt and F0.txt
/usr/local/bin/llc -filetype=obj _tanh_Merged_swapped.ll -o _tanh_Merged_swapped.o
/usr/local/bin/clang++ runFunc.cpp _tanh_Merged_swapped.o -o runFunc -lm
./runFunc T 1 _tanh_Merged
./runFunc F 0 _tanh_Merged

rm -rf *.o runFunc

# Check mergeability
/usr/local/bin/clang++ isMergeable.cpp -o isMergeable -lm
isMergeable=$(./isMergeable 6)
echo "isMergeable? : $isMergeable"

if [ "$isMergeable" -eq 0 ] || [ "$isMergeable" -eq 1 ]; then
    echo -e "\n[*] Apply Merge and SimplifyCFG pass on tanh\n"
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libMerge.so -passes=merge-path,simplifycfg -merge-direction=$isMergeable _tanh_Merged.ll -o _tanh_Merged2.bc
    /usr/local/bin/llvm-dis _tanh_Merged2.bc
    
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libMerge.so -passes=dot-cfg -disable-output _tanh_Merged2.ll
    if dot -Tpng -o ./assets/_tanh_Merged2.png ./._tanh.dot;then
        echo -e "\n[*] A new CFG is drawn at ./assets/_tanh_Merged2.png!"
    else
        echo -e "\n[*] Fail to draw a new CFG :("
    fi
fi