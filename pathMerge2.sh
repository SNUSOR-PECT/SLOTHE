#!/bin/bash

# Unreachable path -> input range

# Check mergeability
# /usr/local/bin/clang++ isMergeable.cpp -o isMergeable -lm
# isMergeable=$(./isMergeable 6)
# echo "isMergeable? : $isMergeable"

# if [ $isMergeable -eq 1 ];then
#     echo -e "\n[*] Apply Merge and SimplifyCFG pass on tanh\n"
#     /usr/local/bin/opt -load-pass-plugin ./build/lib/libMerge.so -passes=merge-path,simplifycfg _tanh_UnreachablePath.ll -o _tanh_Merged.bc
#     /usr/local/bin/llvm-dis _tanh_Merged.bc
    
#     /usr/local/bin/opt -load-pass-plugin ./build/lib/libMerge.so -passes=dot-cfg -disable-output _tanh_Merged.ll
#     if dot -Tpng -o ./assets/_tanh_Merged.png ./._tanh.dot;then
#         echo -e "\n[*] A new CFG is drawn at ./assets/_tanh_Merged.png!"
#     else
#         echo -e "\n[*] Fail to draw a new CFG :("
#     fi
# fi

echo -e "\n[*] Apply Merge and SimplifyCFG pass on tanh\n"
/usr/local/bin/opt -load-pass-plugin ./build/lib/libMerge.so -passes=merge-path,simplifycfg _tanh_Merged.ll -o _tanh_Merged2.bc
/usr/local/bin/llvm-dis _tanh_Merged2.bc

/usr/local/bin/opt -load-pass-plugin ./build/lib/libMerge.so -passes=dot-cfg -disable-output _tanh_Merged2.ll
if dot -Tpng -o ./assets/_tanh_Merged2.png ./._tanh.dot;then
    echo -e "\n[*] A new CFG is drawn at ./assets/_tanh_Merged2.png!"
else
    echo -e "\n[*] Fail to draw a new CFG :("
fi