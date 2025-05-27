#!/bin/bash

if [ "$#" -ne 7 ]; then
  echo "Usage: $0 <NAF> <source> <precision> <time> <min> <max> <Mode>"
  exit 1
fi

# Build NAF list
declare -A NAF
NAF[tanh]='tanh(x)'
NAF[gelu]='x/2*(1+erf(x/sqrt(2)))'
NAF[erf]='erf(x)'
NAF[expm1]='expm1(x)'

# 0. Op-Analyzer (1) replace constant fdiv into fmul
/usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceDivtoMul.so -passes=replace-div-mul,dce -S $2 -o temp/$1_tmp.ll

# 1. Op-Analyzer (2) build approximation queue
approxQueue=($(/usr/local/bin/opt -load-pass-plugin ./build/lib/libOpAnalyzer.so -passes=analyze-op,dce -S -disable-output temp/$1_tmp.ll 2>&1))

# Signal [00] return IRB_{old}
for f in "${approxQueue[@]}"; do
  if [[ $f == "-1" ]]; then
    echo "[*] sub-func has a incomputable operation"
    echo "[*] abort the FBA and return IRB_{old}"
    echo "00" > temp/signal.txt
    exit 1
  fi
done

for f in "${approxQueue[@]}"; do

  # 1. get input range of the sub-func $f
  /usr/local/bin/opt -load-pass-plugin ./build/lib/libGetFuncRange.so -passes=get-func-range,dce -target-func=$f -S -o temp/temp_$f.ll temp/$1_tmp.ll
  /usr/local/bin/llc -filetype=obj temp/temp_$f.ll -o temp_$f.o
  /usr/local/bin/clang++ ./scripts/checkMax.cpp temp_$f.o -o checkMax -lm
  fMax=$(./checkMax _$1 $5 $6)
  echo "function $f, ($5, $6) fMax : $fMax"

  found=0
  for deg in $(seq 14 1 27); do
    # 2. run PAG with desired input range of subfunction
    # TODO: modify lolremez output (function) name
    lolremez --double -r "-$fMax:$fMax" "${NAF[$f]}" -d $deg > temp/temp_$f.c

    # 3. compile PA
    /usr/local/bin/clang -O2 -c -emit-llvm temp/temp_$f.c -o temp/temp_$f.bc
    /usr/local/bin/llvm-dis temp/temp_$f.bc
    /usr/local/bin/opt -disable-output temp/temp_$f.ll

    # 4. New IRB structure of $2 by linking $2 and PA
    /usr/local/bin/llvm-link temp/$1_tmp.ll temp/temp_$f.ll -S -o temp/merged.ll
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceFunc.so -passes=replace-func,dce -source-func=$f -target-func=f -S -o temp/replaced.ll temp/merged.ll

    # 5-1. (UDC-MinErr) check the validity of current PA for sub-func
    condErr=$(bash ./scripts/checkValid.sh $1 replaced $3 $5 $6)
    fRate=$(echo "$condErr" | awk '{print $4}' | tr -d '%')
    if [[ "$fRate" == "0" ]]; then
      echo "Minimum approximation degree of $f = $deg"
      found=1
      # update IRB state
      cp temp/replaced.ll temp/$1_tmp.ll
      break
    fi
  done
done

if [[ $found -eq 0 ]]; then
  # echo "PA is not found."
  # (signal) e_{new} > e_{prev} : signal <- "keep IRB_{old} and select next IRB"
  echo "[*] keep IRB_{old} and select next IRB"
  echo "01" > temp/signal.txt
  exit 1
fi

# Division replacement
isExistDiv=$(
  /usr/local/bin/opt -load-pass-plugin ./build/lib/libGetDivisorRange.so \
    -passes=get-div-range,dce -target-func=_"$1" -S \
    -o temp/$1_tmp_div.ll temp/$1_tmp.ll 2>&1
)

if [[ $isExistDiv == "1" ]]; then
  /usr/local/bin/llc -filetype=obj temp/$1_tmp_div.ll -o $1_tmp_div.o
  /usr/local/bin/clang++ ./scripts/checkMax.cpp $1_tmp_div.o -o checkMax -lm
  divMax=$(./checkMax _$1 $5 $6)
  echo "divMax : $divMax"

  # 6. compile approximated divide (inverse)
  /usr/local/bin/clang -O2 -c -emit-llvm math/_inverse.c -o temp/_inverse.bc
  /usr/local/bin/llvm-dis temp/_inverse.bc
  /usr/local/bin/opt -disable-output temp/_inverse.ll

  # 7. link top function and approximated polynomial
  /usr/local/bin/llvm-link temp/$1_tmp.ll temp/_inverse.ll -S -o temp/merged_div_$1.ll

  found2=0
  for d in $(seq 11 1 17); do
    # 8. run ReplaceDiv pass
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceDiv.so -passes=replace-div,dce -div-max=$divMax -iter-d=$d -S -o temp/temp_div_$1.ll temp/merged_div_$1.ll

    # 5. check the validity of current approximated polynomial for sub-func
    tmp=$(bash ./scripts/checkValid.sh $1 temp_div_$1 $3 $5 $6)
    fRate2=$(echo "$tmp" | awk '{print $4}' | tr -d '%')
    if [[ "$fRate2" == "0" ]]; then
        echo "Minimum d = $d"
        found2=1
        cp temp/temp_div_$1.ll temp/$1_tmp.ll
        break
    fi
  done

  if [[ $found2 -eq 0 ]]; then
    # (signal) e_{new} > e_{prev} : signal <- "keep IRB_{old} and continue the loop"
    echo "[*] Div is not able to be replaced."
    echo "01" > temp/signal.txt
    exit 1
  fi
fi

if [[ $7 == "minErr" ]]; then
  condTime=$(bash ./scripts/checkTime.sh $1 $1_tmp $4 $5 $6)
  cond=$condTime
else # M == "minTime"
  cond=$found
fi

echo "M=$7, cond = $cond"

if [[ $cond -eq "1" ]]; then
  if [[ $7 == "minErr" ]]; then
      # (signal) signal <- "terminate FBA and return IRB_{old}"
      echo "[*] terminate FBA and return IRB_{old}"
      echo "03" > temp/signal.txt
    else
      # (signal) signal <- "terminate FBA and return IRB_{tmp}"
      echo "[*] terminate FBA and return IRB_{tmp}"
      echo "04" > temp/signal.txt
    fi
else
  # (signal) signal <- "IRB_{old}=IRB{tmp} and pass sub-func"
  echo "[*] pass sub-func to op-analyzer and push IRBs"
  echo "02" > temp/signal.txt

  for subF in "${approxQueue[@]}"; do
    # (1) run cf-optimizer on $subF
    # bash ./scripts/cf_optimizer.sh $subF $2 $4 $5
    /usr/local/bin/clang -O2 -c -emit-llvm ./math/_$subF.c -o ./temp/_"$subF"_optim.bc
    /usr/local/bin/llvm-dis ./temp/_"$subF"_optim.bc

    # (2) merge $1_tmp and optimized $subF
    /usr/local/bin/llvm-link temp/$1_tmp.ll temp/_"$subF"_optim.ll -S -o temp/$1_tmp_$subF.ll
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceFunc.so -passes=replace-func,dce -source-func=f -target-func=_$subF -S -o temp/$1_cand_$subF.ll temp/$1_tmp_$subF.ll

    # (3) push $1_tmp+$subF merged IRB into fba_pool
    cp temp/$1_cand_$subF.ll fba_pool/$1_cand_$subF.ll
  done
fi

rm -rf *.o checkMax