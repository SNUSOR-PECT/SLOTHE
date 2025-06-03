#!/bin/bash

if [ "$#" -ne 8 ]; then
  echo "Usage: $0 <NAF> <source> <precision> <time> <min> <max> <Mode> <HE-precision>"
  exit 1
fi

# Build NAF list
declare -A NAF
NAF[tanh]='tanh(x)'
NAF[gelu]='x/2*(1+erf(x/sqrt(2)))'
NAF[erf]='erf(x)'
NAF[expm1]='expm1(x)'
NAF[swish]='x/(exp(-x)+1)'
NAF[sigmoid]='1/(exp(-x)+1)'
NAF[exp]='exp(x)'

# 0. Op-Analyzer (1) replace constant fdiv into fmul
/usr/bin/opt-16 -load-pass-plugin ./build/lib/libReplaceDivtoMul.so -passes=replace-div-mul,dce -S $2 -o temp/$1_tmp.ll

# 1. Op-Analyzer (2) build approximation queue
# approxQueue=($(/usr/bin/opt-16 -load-pass-plugin ./build/lib/libOpAnalyzer.so -passes=analyze-op,dce -target-func=_"$1" -S -disable-output temp/$1_tmp.ll 2>&1))
approxQueue=($(/usr/bin/opt-16 -load-pass-plugin ./build/lib/libOpAnalyzer.so -passes=analyze-op,dce -S -disable-output temp/$1_tmp.ll 2>&1))

# for f in "${approxQueue[@]}"; do
#   echo "f = $f"
# done

# 2. run PAG for AQ (Division)
isExistDiv=$(
  /usr/bin/opt-16 -load-pass-plugin ./build/lib/libGetDivisorRange.so \
    -passes=get-div-range,dce -target-func=_"$1" -S \
    -o temp/$1_tmp_div.ll temp/$1_tmp.ll 2>&1
)

if [[ $isExistDiv == "1" ]]; then
  /usr/bin/llc-16 -filetype=obj temp/$1_tmp_div.ll -o $1_tmp_div.o
  /usr/bin/clang++-16 ./utils/checkMax.cpp $1_tmp_div.o -o checkMax -lm
  divMax=$(./checkMax _$1 $5 $6)
  # echo "divMax : $divMax"

  # 6. compile approximated divide (inverse)
  /usr/bin/clang-16 -O2 -c -emit-llvm math/_inverse.c -o temp/_inverse.bc
  /usr/bin/llvm-dis-16 temp/_inverse.bc
  /usr/bin/opt-16 -disable-output temp/_inverse.ll

  # 7. link top function and approximated polynomial
  /usr/bin/llvm-link-16 temp/$1_tmp.ll temp/_inverse.ll -S -o temp/merged_div_$1.ll

  found2=0
  expNum=$(tail -c 2 temp/errPrev.txt | head -c 1)
  for d in $(seq 11 1 17); do
    # 8. run ReplaceDiv pass
    /usr/bin/opt-16 -load-pass-plugin ./build/lib/libReplaceDiv.so -passes=replace-div,dce -div-max=$divMax -iter-d=$d -S -o temp/temp_div_$1.ll temp/merged_div_$1.ll

    # 5. check the validity of current approximated polynomial for sub-func
    tmp=$(bash ./scripts/checkFunctionalValid.sh $1 temp_div_$1 $expNum $5 $6)
    fRate2=$(echo "$tmp" | awk '{print $4}' | tr -d '%')
    if [[ "$fRate2" == "0" ]]; then
        # echo "Minimum d = $d"
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

  # check err
  errNew=$(bash ./scripts/checkErr.sh $1 $1_tmp $5 $6)
  read -r errPrev < temp/errPrev.txt

  _errNew=$(printf "%.10f" "$errNew")
  _errPrev=$(printf "%.10f" "$errPrev")

  if (( $(echo "$_errNew > $_errPrev" | bc -l) )); then
    echo "01" > temp/signal.txt
    exit 1
  fi

  # update err_{prev} -> not update
  # echo "$errNew" > temp/errPrev.txt
fi

# Signal [00] return IRB_{old}
for f in "${approxQueue[@]}"; do
  if [[ $f == "-1" ]]; then
    echo "[*] sub-func has an incomputable operation"
    echo "[*] abort the FBA and return IRB_{old}"
    echo "00" > temp/signal.txt
    exit 1
  fi
done

# run PAG for each sub-func
for f in "${approxQueue[@]}"; do

  # 1. get input range of the sub-func $f
  /usr/bin/opt-16 -load-pass-plugin ./build/lib/libGetFuncRange.so -passes=get-func-range,dce -target-func=$f -S -o temp/temp_$f.ll temp/$1_tmp.ll
  /usr/bin/llc-16 -filetype=obj temp/temp_$f.ll -o temp_$f.o
  /usr/bin/clang++-16 ./utils/checkMax.cpp temp_$f.o -o checkMax -lm
  fMax=$(./checkMax _$1 $5 $6)
  # echo "function $f, ($5, $6) fMax : $fMax"

  found=0
  read -r errPrevTop < temp/errPrev.txt
  _errPrevTop=$(printf "%.10f" "$errPrevTop")
  
  for (( _deg=10; _deg<=27; )); do
    # 2. run PAG with desired input range of subfunction
    lolremez --double -r "-$fMax:$fMax" "${NAF[$f]}" -d $_deg > temp/temp_$f.c

    # check if the coeff can be encoded
    pval=$(./checkPrecVal temp/temp_$f.c $8)
    # echo "deg=$_deg, coeff can be represented? $pval"
    if [[ $pval == "0" ]]; then
      (( _deg++ ))
      continue
    fi

    # 3. compile PA
    /usr/bin/clang-16 -O2 -c -emit-llvm temp/temp_$f.c -o temp/temp_$f.bc
    /usr/bin/llvm-dis-16 temp/temp_$f.bc
    /usr/bin/opt-16 -disable-output temp/temp_$f.ll

    # 4. New IRB structure of $2 by linking $2 and PA
    /usr/bin/llvm-link-16 temp/$1_tmp.ll temp/temp_$f.ll -S -o temp/merged.ll
    /usr/bin/opt-16 -load-pass-plugin ./build/lib/libReplaceFunc.so -passes=replace-func,dce -source-func=$f -target-func=f -S -o temp/replaced.ll temp/merged.ll

    # 5-1. (UDC-tracker) check the validity of current PA for sub-func (best option)
    if [[ $7 == "minErr" ]]; then
      condTime=$(bash ./scripts/checkTime.sh $1 replaced $4)

      errNew=$(bash ./scripts/checkErr.sh $1 replaced $5 $6)
      _errNew=$(printf "%.10f" "$errNew")
      _errPrev=$(printf "%.10f" "$errPrev")

      # new IRB should have better error than previous IRB
      # echo "_deg=$_deg, saved deg=$deg |  $_errPrevTop >= $_errNew ($(echo "$_errPrevTop >= $_errNew" | bc -l)) | $condTime, $found"
      if (( $(echo "$_errPrevTop >= $_errNew" | bc -l) )); then

        # check if time is not met
        if [[ $condTime == "1" ]]; then
          # echo "--- Time not met & break ---"
          errNew=$errPrev
          deg=$_deg-1
          # (( _deg-- )) # keep prior IRB (temp/$1_tmp.ll)
          break
        fi

        if [[ $found == 0 ]]; then
        # echo "--- Time met ---"
          # update
          errPrev=$errNew
          found=1
          deg=$_deg
          cp temp/temp_"$f".c temp/temp_"$f"_old.c
          cp temp/replaced.ll temp/$1_replaced_tmp.ll
          # (( _deg++ ))
          # continue
        fi

        # check if err is not better than the previous replaced, save the prior IRB
        if [[ $(echo "$_errPrev <= $_errNew" | bc -l) ]]; then
          errNew=$errPrev
          deg=$_deg-1
          # (( _deg-- )) # keep prior IRB (temp/$1_replaced_tmp.ll)
          break
        fi
      fi

    else # $7 == "minTime"
      # run checkErr
      condErr=$(bash ./scripts/checkFunctionalValid.sh $1 replaced $3 $5 $6)
      fRate=$(echo "$condErr" | awk '{print $4}' | tr -d '%')
      if [[ "$fRate" == "0" ]]; then
        found=1
        errNew=$(bash ./scripts/checkErr.sh $1 replaced $5 $6)
        deg=$_deg
        # update IRB state
        cp temp/temp_"$f".c temp/temp_"$f"_old.c
        cp temp/replaced.ll temp/$1_replaced_tmp.ll
        break
      fi
    fi
    (( _deg++ ))
  done
done

if [[ $_deg == 28 ]]; then
  deg=27
fi

if [[ $found == 0 ]]; then
  echo "[*] PA is not found"
  echo "01" > temp/signal.txt
  exit 1
fi

cp temp/temp_"$f"_old.c temp/temp_"$f".c

# update err_{new}
# echo "degree = $deg"
echo "$errNew" > temp/errPrev.txt
cp temp/$1_replaced_tmp.ll temp/$1_tmp.ll

if [[ $7 == "minErr" ]]; then
  condTime=$(bash ./scripts/checkTime.sh $1 $1_tmp $4)
  cond=$condTime
else # M == "minTime"
  cond=$found
fi

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
    # echo "subF = $subF"
    # (1) run cf-optimizer on $subF
    # bash ./scripts/cf_optimizer.sh $subF $3 $5 $6
    /usr/bin/clang-16 -O2 -c -emit-llvm ./math/_$subF.c -o ./temp/_"$subF"_optim.bc
    /usr/bin/llvm-dis-16 ./temp/_"$subF"_optim.bc

    # (2) merge $1_tmp and optimized $subF
    /usr/bin/llvm-link-16 temp/$1_tmp.ll temp/_"$subF"_optim.ll -S -o temp/$1_tmp_$subF.ll
    /usr/bin/opt-16 -load-pass-plugin ./build/lib/libReplaceFunc.so -passes=replace-func,dce -source-func=f -target-func=_$subF -S -o temp/$1_cand_$subF.ll temp/$1_tmp_$subF.ll

    # (3) push $1_tmp+$subF merged IRB into fba_pool
    cp temp/$1_cand_$subF.ll fba_pool/$1_cand_$subF.ll
  done
fi