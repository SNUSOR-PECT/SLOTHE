#!/bin/bash

# 0. make directory for temporary files
mkdir temp

# 1. Prepare baseline
bash ./scripts/Baseline.sh

# 2. Apply Unreachable LLVM-Path
bash ./scripts/Unreachable.sh

# 3. Check mergeability twice
bash ./scripts/pathMerge.sh _tanh_Unreachable _tanh_Merged1
bash ./scripts/pathMerge.sh _tanh_Merged1 _tanh_Merged2

# 4. Check the validity of baseline function and optimized function
bash ./scripts/checkValid.sh _tanh_Merged2

# 5. (Optional) Apply additional optimization
bash ./scripts/Optional.sh _tanh_Merged2 optim
bash ./scripts/checkValid.sh optim

# 6. Clean files
rm -rf temp/*
rm -rf ./.*.dot
rm -rf T0.txt T1.txt F0.txt F1.txt
# find . -maxdepth 1 -type f -executable -exec rm {} \;
