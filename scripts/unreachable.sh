#!/bin/bash

if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <NAF> <min> <max>"
  exit 1
fi

echo -e "[*] Apply UnreachablePath and SimplifyCFG pass on $1"

/usr/bin/opt-16 -load-pass-plugin ./build/lib/libUnreachable.so -passes=remove-unreachable,simplifycfg -val-min=$2 -val-max=$3 ./temp/_$1.ll -o ./temp/_$1_Unreachable.bc
/usr/bin/llvm-dis-16 ./temp/_$1_Unreachable.bc

/usr/bin/opt-16 -load-pass-plugin ./build/lib/libUnreachable.so -passes=dot-cfg -disable-output ./temp/_$1_Unreachable.ll
/usr/bin/dot -Tpng -o ./results/_$1_Unreachable.png ./._$1.dot
# if /usr/bin/dot -Tpng -o ./results/_$1_Unreachable.png ./._$1.dot;then
#     echo -e "[*] A new CFG is drawn at ./results/_$1_Unreachable.png!"
# else
#     echo -e "[*] Fail to draw a new CFG :("
# fi