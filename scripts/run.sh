#!/bin/bash

if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <precision> <min> <max>"
  exit 1
fi

# 0. make directory for temporary files
mkdir temp

# 1. Prepare baseline
bash ./scripts/baseline.sh

# 2. Apply Unreachable LLVM-Path
bash ./scripts/unreachable.sh

# 3. Check mergeability twice
bash ./scripts/pathMerge.sh _tanh_Unreachable _tanh_Merged1
bash ./scripts/pathMerge.sh _tanh_Merged1 _tanh_Merged2

# 4. Check the validity of baseline function and optimized function
bash ./scripts/checkValid.sh _tanh_Merged2 $1 $2 $3

# 5. (Optional) Apply additional optimization
bash ./scripts/optional.sh _tanh_Merged2 optim
bash ./scripts/checkValid.sh optim $1 $2 $3

