#!/bin/bash

# Generate T0.txt and F1.txt
/usr/local/bin/llc -filetype=obj _tanh_Merged2.ll -o _tanh_Merged2.o
/usr/local/bin/clang++ checkPrec.cpp _tanh_Merged2.o -o checkPrec -lm
./checkPrec

rm -rf *.o runFunc
