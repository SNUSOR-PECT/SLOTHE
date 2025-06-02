#!/bin/bash

if [ "$#" -ne 4 ]; then
  echo "Usage: $0 <NAF> <precision> <min> <max>"
  exit 1
fi

mkdir -p temp

src=math/_$1.c
echo "[*] The source file is located at $src"

# 1. IR Translation -> ./temp/_$1.ll
out=$(bash ./scripts/baseline.sh $1 2>&1)
cnt=$(echo "$out" | grep -x '[0-9]\+')
# echo "cnt = $cnt"

if [[ $cnt != 0 ]]; then
  echo "[*] $1 has condition clauses, run CF-optimizer"

  # 2. CF-Optimizer (1) Unrechable path elimination
  bash ./scripts/unreachable.sh $1 $3 $4

  # 3. CF-Optimizer (2) Equivalent path merging
  # TODO: run pathMerge.sh until all branches are analyzed
  bash ./scripts/pathMerge.sh $1 _$1_Unreachable _$1_Merged $2

  # 4. Check the validity of optimized IRB compared to initial IRB
  bash ./scripts/checkValid.sh $1 _$1_Merged $2 $3 $4

  # 5. (Optional) Apply additional optimization
  if [[ $1 == "tanh" ]]; then
      bash ./scripts/optional.sh $1 _$1_Merged $1_optim
      bash ./scripts/checkValid.sh $1 $1_optim $2 $3 $4
  else
      mv temp/_$1_Merged.ll temp/$1_optim.ll
  fi
else
  echo "[*] $1 does not have condition clause, pass CF-optimizer"
  mv temp/_$1.ll temp/$1_optim.ll
fi