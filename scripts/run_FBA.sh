#!/bin/bash

if [ "$#" -ne 6 ]; then
  echo "Usage: $0 <source> <target sub-func> <output> <precision> <min> <max>"
  exit 1
fi

DIR=./temp/
if [ ! -d $DIR ]; then
    mkdir temp
fi

# <source> = initial FBA candidate IRB

# TODO : list up candidates of PAG (OP-Analyzer)
subfuncTbl=($(/usr/local/bin/opt -load-pass-plugin ./build/lib/libOpAnalyzer.so -passes=analyze-op,dce -S -disable-output $1 2>&1))

for f in "${subfuncTbl[@]}"; do
  # 1. get input range of the sub-func
  /usr/local/bin/opt -load-pass-plugin ./build/lib/libGetFuncRange.so -passes=get-func-range,dce -target-func=$f -S -o temp/temp_$f.ll $1
  /usr/local/bin/llc -filetype=obj temp/temp_$f.ll -o temp_$f.o
  /usr/local/bin/clang++ ./scripts/checkMax.cpp temp_$f.o -o checkMax -lm
  fMax=$(./checkMax $5 $6)
  echo "($5, $6) fMax : $fMax"

  for deg in $(seq 11 2 27); do
    # 2. run lolremez with desired input range of subfunction
    # TODO: lolremez output (function) name
    lolremez --double -r "-$fMax:$fMax" "$f(x)" -d $deg > temp/temp_$f.c

    # 2. compile approximated polynomial
    /usr/local/bin/clang -O2 -c -emit-llvm temp/temp_$f.c -o temp/temp_$f.bc
    /usr/local/bin/llvm-dis temp/temp_$f.bc
    /usr/local/bin/opt -disable-output temp/temp_$f.ll

    # 3. link top function and approximated polynomial
    /usr/local/bin/llvm-link $1 temp/temp_$f.ll -S -o temp/merged.ll

    # 4. run ReplaceFunc pass
    # - NAF is replaced with approximated function
    /usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceFunc.so -passes=replace-func,dce -target-func=$f -S -o temp/replaced.ll temp/merged.ll

    # 5. check the validity of current approximated polynomial for sub-func
    tmp=$(bash ./scripts/checkValid.sh replaced $4 $5 $6)
    fRate=$(echo "$tmp" | awk '{print $4}' | tr -d '%')
    if [[ "$fRate" == "0" ]]; then
        echo "Minimum approximation degree of $f = $deg"
        break
    fi
  done
done

# 5. find maximum value of the input of div
# Output
# - NAF is replaced with approximated function
# - The function returns the input of div
/usr/local/bin/opt -load-pass-plugin ./build/lib/libGetDivisorRange.so -passes=get-div-range,dce -S -o temp/temp_$3.ll temp/replaced.ll
/usr/local/bin/llc -filetype=obj temp/temp_$3.ll -o temp_$3.o
/usr/local/bin/clang++ ./scripts/checkMax.cpp temp_$3.o -o checkMax -lm
divMax=$(./checkMax $5 $6)
echo "divMax : $divMax"

# 6. compile approximated divide (inverse)
/usr/local/bin/clang -O2 -c -emit-llvm math/_inverse.c -o temp/_inverse.bc
/usr/local/bin/llvm-dis temp/_inverse.bc
/usr/local/bin/opt -disable-output temp/_inverse.ll

# 7. link top function and approximated polynomial
/usr/local/bin/llvm-link temp/replaced.ll temp/_inverse.ll -S -o temp/merged_div.ll

for d in $(seq 11 1 17); do
  # 8. run ReplaceDiv pass
  /usr/local/bin/opt -load-pass-plugin ./build/lib/libReplaceDiv.so -passes=replace-div,dce -div-max=$divMax -iter-d=$d -S -o temp/result.ll temp/merged_div.ll

  # 5. check the validity of current approximated polynomial for sub-func
  tmp=$(bash ./scripts/checkValid.sh result $4 $5 $6)
  fRate2=$(echo "$tmp" | awk '{print $4}' | tr -d '%')
  if [[ "$fRate2" == "0" ]]; then
      echo "Minimum d = $d"
      cp temp/result.ll results/
      echo "[*] The result is created in results/result.ll !"
      break
  fi
done

rm -rf *.o checkMax