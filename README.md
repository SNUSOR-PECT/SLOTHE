SLOTHE : Lazy Approximation of Non-Arithmetic Neural Network Functions over Encrypted Data
=========

based on **LLVM 16**

### Build (We assume that llvm16 is installed at /usr/local)
```bash
bash ./scripts/build.sh
```

### Run FHE-aware Code Optimization on Tanh
```bash
# bash ./scripts/run.sh <precision> <min> <max>
bash ./scripts/run.sh 4 -4 4
```

### Run FBA on optimized IRB of Tanh
```bash
# bash ./scripts/run_FBA.sh <source> <target sub-func> <output> <precision> <min> <max>
bash ./scripts/run_FBA.sh temp/optim.ll expm1 new_tanh 4 -4 4
```

### Clean all generated temporal files
```bash
bash ./scripts/clean.sh
```
