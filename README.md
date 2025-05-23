SLOTHE : Lazy Approximation of Non-Arithmetic Neural Network Functions over Encrypted Data
=========

based on **LLVM 16**

### Build (We assume that llvm16 is installed at /usr/local)
```bash
bash ./scripts/build.sh
```

### Run SLOTHE on NAF
```bash
# bash ./scripts/SLOTHE.sh <NAF> <precision> <time> <min> <max> 
bash ./scripts/SLOTHE.sh gelu 2 40 -4 4 
bash ./scripts/SLOTHE.sh tanh 2 40 -4 4
bash ./scripts/SLOTHE.sh tanh 4 40 -4 4 
```

### Clean all generated temporal files
```bash
bash ./scripts/clean.sh
```
