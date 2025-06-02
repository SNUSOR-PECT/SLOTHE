#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <NAF>"
  exit 1
fi

echo -e "\n[*] Draw a CFG of a target function ($1)"

/usr/bin/clang-16 -O2 -fno-inline -c -emit-llvm ./math/_$1.c -o ./temp/_$1.bc
/usr/bin/llvm-dis-16 ./temp/_$1.bc
/usr/bin/opt-16 -load-pass-plugin ./build/lib/libCountCond.so -passes=count-conditions -target-func=_$1 -disable-output ./temp/_$1.ll

/usr/bin/opt-16 -passes=dot-cfg -disable-output ./temp/_$1.ll
dot -Tpng -o ./results/_$1.png ./._$1.dot

# if dot -Tpng -o ./results/_$1.png ./._$1.dot; then
#     echo -e "[*] A new CFG is drawn at ./results/_$1.png!"
# else
#     echo -e "[*] Fail to draw a new CFG :("
# fi