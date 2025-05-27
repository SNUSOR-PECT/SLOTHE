#!/bin/bash

if [ "$#" -ne 5 ]; then
  echo "Usage: $0 <NAF> <input> <time> <min> <max>"
  exit 1
fi

/usr/local/bin/opt -load-pass-plugin ./build/lib/libTimeEstimation.so -passes=estimate-time -target-func=_$1 -S -disable-output temp/$2