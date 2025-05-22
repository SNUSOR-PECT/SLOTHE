#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <NAF>"
  exit 1
fi

echo -e "[*] Apply UnreachablePath and SimplifyCFG pass on $1"

/usr/local/bin/opt -load-pass-plugin ./build/lib/libUnreachable.so -passes=remove-unreachable,simplifycfg ./temp/_$1.ll -o ./temp/_$1_Unreachable.bc
/usr/local/bin/llvm-dis ./temp/_$1_Unreachable.bc

/usr/local/bin/opt -load-pass-plugin ./build/lib/libUnreachable.so -passes=dot-cfg -disable-output ./temp/_$1_Unreachable.ll
if dot -Tpng -o ./results/_$1_Unreachable.png ./._$1.dot;then
    echo -e "[*] A new CFG is drawn at ./results/_$1_Unreachable.png!"
else
    echo -e "[*] Fail to draw a new CFG :("
fi