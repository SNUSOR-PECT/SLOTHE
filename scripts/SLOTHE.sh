#!/bin/bash

if [ "$#" -ne 5 ]; then
  echo "Usage: $0 <NAF> <precision> <time> <min> <max>"
  exit 1
fi

if [[ $3 < 40 ]]; then
  echo "[*] The time budget is too small (> 40)"
  exit 1
fi

# make directory for temporary files
mkdir -p temp

# 0. initial UDC checking for NAF itself when replaced with PA
declare -A NAF
NAF[tanh]='tanh(x)'
NAF[gelu]='x/2*(1+erf(x/sqrt(2)))'

precLim="1e-$2"
found=0

for deg in $(seq 14 1 14); do
  # 2. run PAG with desired input range of subfunction
  lolremez --double -r "$4:$5" "${NAF[$1]}" -d $deg > temp/temp_$1.c
  maxerr=$(awk -F':' '/Estimated max error/{
               gsub(/[[:space:]]*/,"",$2); print $2}' temp/temp_"$1".c)

  dec_err=$(printf "%.20f" "$maxerr")
  dec_lim=$(printf "%.20f" "$precLim")

  if (( $(echo "$dec_err < $dec_lim" | bc -l) )); then
    echo "$maxerr < $precLim  →  target met"
    found=1
    break
  fi
done

# if time budget remains
if [[ $found == "1" ]]; then
  echo "[*] run SLOTHE"
  # 1. run CF-optimizer
  bash ./scripts/cf_optimizer.sh $1 $2 $4 $5

  # initial IRB is "temp/$1_optim.ll"
  cost=$(/usr/local/bin/opt -load-pass-plugin ./build/lib/libCostEstimation.so -passes=estimate-cost -S -disable-output temp/$1_optim.ll 2>&1)

  # if time budget remains for definition of $1,
  if [[ $cost < $3 ]]; then
    # 2. run Funtion Body Analysis (FBA)
    bash ./scripts/run_FBA.sh $1 temp/$1_optim.ll $2 $4 $5

    echo "[*] The final IRB is created in results/$1_result.ll !"

  # if not, abort and return previous PA result
  else
    echo "[*] Estimated cost of $1 is $cost, which exceeds $3, abort the SLOTHE."
    echo "[*] SLOTHE results in degree $deg PA for $1 at temp/temp_"$1".c."
  fi
else
  echo "[*] SLOTHE results in degree $deg PA for $1  at temp/temp_"$1".c."
  echo "[*] Estimated max error is $maxerr"
fi