#!/bin/bash

if [ "$#" -ne 5 ]; then
  echo "Usage: $0 <NAF> <source> <precision> <min> <max>"
  exit 1
fi

# Build NAF list
declare -A NAF
NAF[tanh]='tanh(x)'
NAF[gelu]='x/2*(1+erf(x/sqrt(2)))'
NAF[erf]='erf(x)'
NAF[expm1]='expm1(x)'

# <source> = initial FBA candidate IRB

# 0. Op-Analyzer (1) replace constant fdiv into fmul
/usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceDivtoMul.so -passes=replace-div-mul,dce -S $2 -o temp/temp_$1.ll

# 1. Op-Analyzer (2) build sub-func table and filter candidates of PAG
subfuncTbl=($(/usr/local/bin/opt -load-pass-plugin ./build/lib/libOpAnalyzer.so -passes=analyze-op,dce -S -disable-output temp/temp_$1.ll 2>&1))

# TODO: f order by heuristic cost estimation
for f in "${subfuncTbl[@]}"; do
  # in the definition of $f, if it includes some uncomputable sub-function, abort the FBA
  # -> $f should be approximated with PA
  # TODO check if $f holds uncomputable sub-func

  # 1. get input range of the sub-func $f
  /usr/local/bin/opt -load-pass-plugin ./build/lib/libGetFuncRange.so -passes=get-func-range,dce -target-func=$f -S -o temp/temp_$f.ll temp/temp_$1.ll
  /usr/local/bin/llc -filetype=obj temp/temp_$f.ll -o temp_$f.o
  /usr/local/bin/clang++ ./scripts/checkMax.cpp temp_$f.o -o checkMax -lm
  fMax=$(./checkMax _$1 $4 $5)
  echo "function $f, ($4, $5) fMax : $fMax"

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

    # 5. check the validity of current PA for sub-func (UDC Tracker)
    tmp=$(bash ./scripts/checkValid.sh $1 replaced $3 $4 $5)
    fRate=$(echo "$tmp" | awk '{print $4}' | tr -d '%')
    if [[ "$fRate" == "0" ]]; then
        echo "Minimum approximation degree of $f = $deg"
        found=1
        # update IRB state
        mv temp/replaced.ll temp/temp_$1.ll
        break
    fi
  done

  if [[ $found -eq 0 ]]; then
    echo "PA is not found, pass $f to CF-optimizer"
    bash ./scripts/cf_optimizer.sh $f $1_div -$fMax $fMax
    subfuncTbl+=($(/usr/local/bin/opt -load-pass-plugin ./build/lib/libOpAnalyzer.so -passes=analyze-op,dce -S -disable-output temp/$f_optim.ll 2>&1))

    # Apply 4) New IRB structure of $f by linking $f and PA
    /usr/local/bin/llvm-link temp/temp_$1.ll temp/$f_optim.ll -S -o temp/merged.ll
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceFunc.so -passes=replace-func,dce -target-func=$f -S -o temp/temp_$1.ll temp/merged.ll
  fi
done

# 5. find maximum value of the input of div
# Output
# - NAF is replaced with approximated function
# - The function returns the input of div

# isExistDiv=$(/usr/local/bin/opt -load-pass-plugin ./build/lib/libGetDivisorRange.so -passes=get-div-range,dce -S -o temp/temp_$1_div.ll temp/temp_$1.ll 2>&1 | head -n1)
isExistDiv=$(
  /usr/local/bin/opt -load-pass-plugin ./build/lib/libGetDivisorRange.so \
    -passes=get-div-range,dce -S \
    -o temp/temp_"$1"_div.ll  temp/temp_"$1".ll 2>&1 |
  awk '/^[0-9]+$/ {print; exit}'
)

if [[ $isExistDiv == "1" ]]; then
  /usr/local/bin/llc -filetype=obj temp/temp_$1_div.ll -o temp_$1_div.o
  /usr/local/bin/clang++ ./scripts/checkMax.cpp temp_$1_div.o -o checkMax -lm
  divMax=$(./checkMax _$1 $4 $5)
  echo "divMax : $divMax"

  # 6. compile approximated divide (inverse)
  /usr/local/bin/clang -O2 -c -emit-llvm math/_inverse.c -o temp/_inverse.bc
  /usr/local/bin/llvm-dis temp/_inverse.bc
  /usr/local/bin/opt -disable-output temp/_inverse.ll

  # 7. link top function and approximated polynomial
  /usr/local/bin/llvm-link temp/temp_$1.ll temp/_inverse.ll -S -o temp/merged_div.ll

  for d in $(seq 11 1 17); do
    # 8. run ReplaceDiv pass
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceDiv.so -passes=replace-div,dce -div-max=$divMax -iter-d=$d -S -o temp/$1_result.ll temp/merged_div.ll

    # 5. check the validity of current approximated polynomial for sub-func
    tmp=$(bash ./scripts/checkValid.sh $1 $1_result $3 $4 $5)
    fRate2=$(echo "$tmp" | awk '{print $4}' | tr -d '%')
    if [[ "$fRate2" == "0" ]]; then
        echo "Minimum d = $d"
        cp temp/$1_result.ll results/
        break
    fi
  done
else
  cp temp/temp_$1.ll results/$1_result.ll
fi

rm -rf *.o checkMax