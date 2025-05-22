#!/bin/bash

if [ "$#" -ne 4 ]; then
  echo "Usage: $0 <NAF> <precision> <min> <max>"
  exit 1
fi

# 1. make directory for temporary files
if [ ! -d temp ]; then
    mkdir temp
fi

# 1. run CF-optimizer
bash ./scripts/cf_optimizer.sh $1 $2 $3 $4

# 2. run Funtion Body Analysis
bash ./scripts/run_FBA.sh $1 temp/$1_optim.ll $2 $3 $4

echo "[*] The final IRB is created in results/$1_result.ll !"