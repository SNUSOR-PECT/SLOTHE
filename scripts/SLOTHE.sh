#!/bin/bash

if [ "$#" -ne 7 ]; then
  echo "Usage: $0 <NAF> <precision> <time> <min> <max> <mode> <HE-precision>"
  exit 1
fi

# We assume that the time budget is at least on par with that available in PA
if [[ $3 < 10 ]]; then
  echo "[*] The time budget is too small (> 40)"
  exit 1
fi

# make directory for temporary files
mkdir -p temp
mkdir -p fba_pool

# 0. initial UDC checking if NAF can be replaced with PA
declare -A NAF
NAF[tanh]='tanh(x)'
NAF[gelu]='x/2*(1+erf(x/sqrt(2)))'

precLim="1e-$2"
/usr/local/bin/clang++ ./utils/checkPrecVal.cpp -o checkPrecVal -lm

for (( _deg=10; _deg<=27; )); do
  # 2. run PAG with desired input range of subfunction
  lolremez --double -r "$4:$5" "${NAF[$1]}" -d $_deg > temp/temp_$1.c
  
  # check if the coeff can be encoded
  pval=$(./checkPrecVal temp/temp_$1.c $7)
  # echo "deg=$_deg, coeff can be represented? $pval"
  if [[ $pval == "0" ]]; then
    (( _deg++ ))
    continue
  fi

  maxerr=$(awk -F':' '/Estimated max error/{
               gsub(/[[:space:]]*/,"",$2); print $2}' temp/temp_"$1".c)

  dec_err=$(printf "%.10f" "$maxerr")
  dec_lim=$(printf "%.10f" "$precLim")

  if [[ $6 == "minErr" ]]; then
    # will choose the highest _deg
    /usr/local/bin/clang -O2 -c -emit-llvm temp/temp_"$1".c -o temp/$1_tmp.bc
    /usr/local/bin/llvm-dis temp/$1_tmp.bc
    condTime=$(bash ./scripts/checkTime.sh $1 $1_tmp $3)
    if [[ $condTime == "1" ]]; then
      # (( _deg-- )) # save prior IRB
      deg=$_deg-1
      break
    else
      deg=$_deg
      cp temp/temp_"$1".c temp/temp_"$1"_old.c
      cp temp/$1_tmp.ll temp/$1_old.ll
    fi
  else
    # will choose the lowest _deg
    if (( $(echo "$dec_err < $dec_lim" | bc -l) )); then
      /usr/local/bin/clang -O2 -c -emit-llvm temp/temp_"$1".c -o temp/$1_old.bc
      /usr/local/bin/llvm-dis temp/$1_old.bc
      cp temp/temp_"$1".c temp/temp_"$1"_old.c
      deg=$_deg
      # echo "$maxerr < $precLim  →  target met"
      break
    fi
  fi

  (( _deg++ ))
done

if [[ $_deg == 28 ]]; then
  deg=27
fi

# echo "deg = $deg, maxerr = $maxerr"
cp temp/temp_"$f"_old.c temp/temp_"$f".c
echo "$maxerr" > temp/errPrev.txt

# 1. run CF-optimizer on $1 -> output name is $1_optim.ll
echo "[*] run SLOTHE"
bash ./scripts/cf_optimizer.sh $1 $2 $4 $5

# initial IRB is "temp/$1_optim.ll", push to fba_pool
cp temp/$1_optim.ll fba_pool/$1_cand1.ll

# Run FBA until fba_pool is empty
shopt -s nullglob
while :; do
  files=( fba_pool/*.ll )
  (( ${#files[@]} )) || break   # empty → leave loop

  IRB_target=""
  cost_target=""

  # Heuristic cost estimation using CostEstimation pass
  for f in "${files[@]}"; do
    costs=$(/usr/local/bin/opt -load-pass-plugin ./build/lib/libCostEstimation.so -passes=estimate-cost -S -disable-output $f 2>&1)
    cost=$(echo "$costs" | awk -F': ' '{sum+=$2} END {print sum}')
    if [[ -z $cost_target || $(awk -v a="$cost" -v b="$cost_target" \
                                  'BEGIN{exit (a<b)?0:1}') == 0 ]]; then
      cost_target=$cost
      IRB_target=$f
    fi
  done

  echo "[*] Lowest cost this round: $IRB_target"

  # run FBA on target (IRB_{tmp} = $IRB_target)
  cp $IRB_target ./temp/$1_tmp.ll
  bash ./scripts/run_FBA.sh $1 ./temp/$1_tmp.ll $2 $3 $4 $5 $6 $7

  # Signal [00] return IRB_{old}
  # Signal [01] keep IRB_{old} and select next IRB
  # Signal [02] IRB_{old} = IRB_{tmp} and pass sub-func to op-analyzer
  # Signal [03] terminate FBA and return IRB_{old}
  # Signal [04] terminate FBA and return IRB_{tmp}

  read -r sig < temp/signal.txt # ← signal
  if [[ $sig == "00" || $sig == "03" || $sig == "04" ]]; then
    break
  elif [[ $sig == "01" ]]; then
    # keep IRB_{old} and continue the loop
    echo "[*] keep IRB_{old} and continue the loop"
  elif [[ $sig == "02" ]]; then
    # update IRB_{old} = IRB_{tmp}
    echo "[*] IRB_{old} = IRB_{tmp} and continue the loop"
    cp ./temp/$1_tmp.ll ./temp/$1_old.ll
  fi

  rm -f -- "$f"
done

if ! compgen -G "path/to/dir/*" > /dev/null; then
  echo "[*] No FBA candidates found."
fi

# print SLOTHE result
echo "[*] Result is saved at results/$1_result.ll ."
if [[ $sig == "00" || $sig == "01" || $sig == "02" || $sig == "03" ]]; then
  cp ./temp/$1_old.ll results/$1_result.ll
elif [[ $sig == "04" ]]; then
  cp ./temp/$1_tmp.ll results/$1_result.ll
fi

bash ./scripts/printResults.sh $1 results/$1_result.ll $4 $5

rm -rf checkPrecVal