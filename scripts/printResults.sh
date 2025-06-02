#!/bin/bash

if [ "$#" -ne 4 ]; then
  echo "Usage: $0 <NAF> <input> <min> <max>"
  exit 1
fi

# time
t=$(/usr/bin/opt-16 -load-pass-plugin ./build/lib/libTimeEstimation.so -passes=estimate-time -target-func=_$1 -param-path=scripts/time.csv -S -disable-output $2 2>&1)

# err
# Generate T0.txt and F1.txt
/usr/bin/llc-16 -filetype=obj $2 -o tmp.o
/usr/bin/clang++-16 utils/checkErr.cpp tmp.o -o checkErr -lm
e=$(./checkErr _$1 $3 $4)

echo "======================================================"
echo "Expected time = $t, expected err = $e"
echo "======================================================"

rm -rf checkErr tmp.o