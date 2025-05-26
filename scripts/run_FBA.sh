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
/usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceDivtoMul.so -passes=replace-div-mul,dce -S $2 -o temp/temp_$1.ll

# 1. Op-Analyzer (2) build approximation queue
approxQueue=($(/usr/local/bin/opt -load-pass-plugin ./build/lib/libOpAnalyzer.so -passes=analyze-op,dce -S -disable-output temp/temp_$1.ll 2>&1))

# Signal [00] return IRB_{old}
if [[ $approxQueue == "-1" ]]; then
  echo "[*] abort the FBA and return IRB_{old}"
  echo "00" > temp/signal.txt
  exit 1
fi

for f in "${approxQueue[@]}"; do

  # 1. get input range of the sub-func $f
  /usr/local/bin/opt -load-pass-plugin ./build/lib/libGetFuncRange.so -passes=get-func-range,dce -target-func=$f -S -o temp/temp_$f.ll temp/temp_$1.ll
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
    /usr/local/bin/llvm-link temp/temp_$1.ll temp/temp_$f.ll -S -o temp/merged.ll
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceFunc.so -passes=replace-func,dce -target-func=$f -S -o temp/replaced.ll temp/merged.ll

    # 5-1. (UDC-MinErr) check the validity of current PA for sub-func
    condErr=$(bash ./scripts/checkValid.sh $1 replaced $3 $5 $6)
    fRate=$(echo "$condErr" | awk '{print $4}' | tr -d '%')
    if [[ "$fRate" == "0" ]]; then
      echo "Minimum approximation degree of $f = $deg"
      found=1
      # update IRB state
      mv temp/replaced.ll temp/temp_$1.ll
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
    -o temp/temp_"$1"_div.ll  temp/temp_"$1".ll 2>&1
)

if [[ $isExistDiv == "1" ]]; then
  /usr/local/bin/llc -filetype=obj temp/temp_$1_div.ll -o temp_$1_div.o
  /usr/local/bin/clang++ ./scripts/checkMax.cpp temp_$1_div.o -o checkMax -lm
  divMax=$(./checkMax _$1 $5 $6)
  echo "divMax : $divMax"

  # 6. compile approximated divide (inverse)
  /usr/local/bin/clang -O2 -c -emit-llvm math/_inverse.c -o temp/_inverse.bc
  /usr/local/bin/llvm-dis temp/_inverse.bc
  /usr/local/bin/opt -disable-output temp/_inverse.ll

  # 7. link top function and approximated polynomial
  /usr/local/bin/llvm-link temp/temp_$1.ll temp/_inverse.ll -S -o temp/merged_div_$1.ll

  found2=0
  for d in $(seq 11 1 17); do
    # 8. run ReplaceDiv pass
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceDiv.so -passes=replace-div,dce -div-max=$divMax -iter-d=$d -S -o temp/temp_div_$1.ll temp/merged_div_$1.ll

    # 5. check the validity of current approximated polynomial for sub-func
    tmp=$(bash ./scripts/checkValid.sh $1 $temp_div_$1 $3 $5 $6)
    fRate2=$(echo "$tmp" | awk '{print $4}' | tr -d '%')
    if [[ "$fRate2" == "0" ]]; then
        echo "Minimum d = $d"
        found2=1
        # cp temp/temp_div_$1.ll ./$1_new.ll
        break
    fi
  done

  if [[ $found2 -eq 0 ]]; then
    echo "Div is not able to be replaced."
    # (signal) e_{new} > e_{prev} : signal <- "keep IRB_{old} and select next IRB"
    exit 1
  fi
fi

if [[ $7 == "minErr" ]]; then
  cond=$found
else # M == "minTime"
  condTime=$(bash ./scripts/checkTime.sh $1 temp_$1 $4 $5 $6)
  cond=$condTime
fi

echo "M=$7, cond = $cond"

if [[ $cond -eq "0" ]]; then
  # (signal) update IRB_{old}: signal <- "IRB_{old}=IRB{tmp} and pass sub-func"
  echo "[*] set IRB_{old}=IRB{tmp} and pass sub-func to op-analyzer"
  echo "02" > temp/signal.txt
else
  if [[ $7 == "minErr" ]]; then
    # (signal) signal <- "terminate FBA and return IRB_{old}"
    echo "[*] terminate FBA and return IRB_{old}"
    echo "03" > temp/signal.txt
  else
    # (signal) signal <- "terminate FBA and return IRB_{tmp}"
    echo "[*] terminate FBA and return IRB_{tmp}"
    echo "04" > temp/signal.txt
  fi
fi

rm -rf *.o checkMax