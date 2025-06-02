#!/bin/bash

if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <NAF> <input> <time>"
  exit 1
fi

t=$(/usr/bin/opt-16 -load-pass-plugin ./build/lib/libTimeEstimation.so -passes=estimate-time -target-func=_$1 -param-path=scripts/time.csv -S -disable-output temp/$2.ll 2>&1)

echo $t > temp/time.txt

if [ "$t" -ge "$3" ]; then
    echo "1"
else
    echo "0"
fi