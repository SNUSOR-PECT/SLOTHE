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

### Run SLOTHE examples on NAF
```bash
bash ./scripts/run_examples.sh
```