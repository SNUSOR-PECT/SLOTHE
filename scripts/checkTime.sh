#!/bin/bash

if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <NAF> <input> <time>"
  exit 1
fi

t=$(/usr/local/bin/opt -load-pass-plugin ./build/lib/libTimeEstimation.so -passes=estimate-time -target-func=_$1 -param-path=scripts/time.csv -S -disable-output temp/$2.ll 2>&1)

if [ "$t" -lt "$3" ]; then
    echo "0"
else
    echo "1"
fi