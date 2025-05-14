#!/bin/bash

# Generate T0.txt and F1.txt
/usr/local/bin/llc -filetype=obj _tanh_UnreachablePath.ll -o _tanh_UnreachablePath.o
/usr/local/bin/clang++ runFunc.cpp _tanh_UnreachablePath.o -o runFunc -lm
./runFunc T 0
./runFunc F 1

# Generate T1.txt and F0.txt
/usr/local/bin/llc -filetype=obj _tanh_Mergeable_swapped.ll -o _tanh_Mergeable_swapped.o
/usr/local/bin/clang++ runFunc.cpp _tanh_Mergeable_swapped.o -o runFunc -lm
./runFunc T 1
./runFunc F 0

rm -rf *.o runFunc

# Check mergeability
/usr/local/bin/clang++ isMergeable.cpp -o isMergeable -lm
# /usr/local/bin/clang++ isMergeable.cpp isMergeable.o -o isMergeable -lm
isMergeable=$(./isMergeable 6)
echo "isMergeable? : $isMergeable"