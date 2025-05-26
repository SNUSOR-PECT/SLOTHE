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
mkdir -p fba_pool

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
    # push IRB to FBA candidates pool
    cp temp/$1_optim.ll fba_pool/$1_cand1.ll

  # if not, abort and return previous PA result
  else
    echo "[*] Estimated cost of $1 is $cost, which exceeds $3, abort the SLOTHE."
    echo "[*] SLOTHE results in degree $deg PA for $1 at temp/temp_"$1".c."
    exit 1
  fi
else
  echo "[*] SLOTHE results in degree $deg PA for $1  at temp/temp_"$1".c."
  echo "[*] Estimated max error is $maxerr"
  exit 1
fi

# Run FBA until fba_pool is empty
shopt -s nullglob
while :; do
  files=( fba_pool/*.ll )
  (( ${#files[@]} )) || break   # empty → leave loop

  IRB_target=""
  cost_target=""

  # Heuristic cost estimation using CostEstimation pass
  for f in "${files[@]}"; do
    cost=$(/usr/local/bin/opt -load-pass-plugin ./build/lib/libCostEstimation.so -passes=estimate-cost -S -disable-output $f 2>&1)
    if [[ -z $cost_target || $(awk -v a="$cost" -v b="$cost_target" \
                                  'BEGIN{exit (a<b)?0:1}') == 0 ]]
    then
        cost_target=$cost
        IRB_target=$f
    fi
  done

  echo "[*] Lowest cost this round: $IRB_target (cost=$cost_target)"
  cp $IRB_target ./temp/$1_old.ll

  # run FBA on ./$1_old.ll
  bash ./scripts/run_FBA.sh $1 $IRB_target $2 $3 $4 $5 "minErr"

  # Signal [00] return IRB_{old}
  # Signal [01] keep IRB_{old} and select next IRB
  # Signal [02] IRB_{old} = IRB_{tmp} and pass sub-func to op-analyzer
  # Signal [03] terminate FBA and return IRB_{old}
  # Signal [04] terminate FBA and return IRB_{tmp}

  read -r sig < temp/signal.txt # ← word ends at first whitespace
  if [[ $sig == "00" || $sig == "03" || $sig == "04" ]]; then
    break
  elif [[ $sig == "01" ]]; then
    # keep IRB_{old} and continue the loop
    echo "keep IRB_{old} and continue the loop"
  elif [[ $sig == "02" ]]; then
    # update IRB_{old} = IRB_{tmp} and run below steps
    echo "IRB_{old} = IRB_{tmp} and run below steps"
  fi

  # if $signal == "IRB_{old}=IRB{tmp} and pass sub-func"
  # (1) bash ./scripts/cf_optimizer.sh $subfunc $2 $4 $5
  # (2) merge optimized $subfunc into $1
  # (3) add merged IRB into fba_pool

  rm -f -- "$f"
done

# print SLOTHE result
echo "[*] Result is saved at results/$1_result.ll ."
if [[ $sig == "00" || $sig == "01" || $sig == "02" || $sig == "03" ]]; then
  cp ./temp/$1_old.ll results/$1_result.ll
elif [[ $sig == "04" ]]; then
  cp ./temp/$1_tmp.ll results/$1_result.ll
fi