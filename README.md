SLOTHE : Lazy Approximation of Non-Arithmetic Neural Network Functions over Encrypted Data
=========


## Requirements
```
llvm 16
cmake >= 3.20
```

### Build (We assume that llvm16 is installed at /usr/local)
```bash
bash ./scripts/build.sh
```

### Run SLOTHE on NAF
```bash
# bash ./scripts/SLOTHE.sh <NAF> <precision> <time> <min> <max> 
# if "minErr", <precision> is not used
# if "minTime", <time> is not used
bash ./scripts/SLOTHE.sh gelu 2 40 -8 8 "minErr"
bash ./scripts/SLOTHE.sh tanh 3 40 -4 4 "minErr"
bash ./scripts/SLOTHE.sh tanh 4 900 -4 4 "minErr"
bash ./scripts/SLOTHE.sh tanh 3 40 -4 4 "minTime"
```

### Clean all generated temporal files
This should be executed before running SLOTHE.sh
```bash
bash ./scripts/clean.sh
```
