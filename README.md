SLOTHE : Lazy Approximation of Non-Arithmetic Neural Network Functions over Encrypted Data
=========

## Environment
* CPU: Intel Xeon W5-2465X (16 cores / 32 threads)
* OS: Ubuntu 22.04

## Requirements
```
cmake >= 3.20
c++17 (gcc 11.4.0)
llvm 16
graphviz
git
```

## Software Dependencies
```
lolremez 0.7
```

### Install 
If you use docker image, you could pass this script.
```bash
sudo bash ./scripts/install.sh
```

### Build
```bash
bash ./scripts/build.sh
```

### Run SLOTHE on NAF
```bash
# bash ./scripts/SLOTHE.sh <NAF> <precision> <time> <min> <max> <Mode> <HE-precision>
# if "minErr", <precision> is not used
# if "minTime", <time> is not used
# Tanh
bash ./scripts/SLOTHE.sh tanh 3 40 -4 4 "minErr" 50
bash ./scripts/SLOTHE.sh tanh 4 900 -4 4 "minErr" 50
bash ./scripts/SLOTHE.sh tanh 3 40 -4 4 "minTime" 50
# Gelu
bash ./scripts/SLOTHE.sh gelu 2 40 -8 8 "minErr" 50
bash ./scripts/SLOTHE.sh gelu 2 50 -8 8 "minErr" 50
# Swish
bash ./scripts/SLOTHE.sh swish 2 40 -8 8 "minErr" 50
bash ./scripts/SLOTHE.sh swish 2 50 -8 8 "minErr" 50
```

### Clean all generated temporal files
This should be executed before running SLOTHE.sh.
```bash
bash ./scripts/clean.sh
```
