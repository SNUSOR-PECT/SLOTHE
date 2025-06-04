SLOTHE : Lazy Approximation of Non-Arithmetic Neural Network Functions over Encrypted Data
=========

## Summary
This repository provides the implementation codes of 'SLOTHE : Lazy Approximation of Non-Arithmetic Neural Network Functions over Encrypted Data' which will be presented in USENIX Security 2025.
The contents are identical with the Artifact in Zenodo.

## Requirements
```
cmake >= 3.20
c++17 (gcc 11.4.0)
llvm 16
graphviz
git
```

## Tested Environment (guaranteed to run on)
* CPU: Intel Xeon W5-2465X (16 cores / 32 threads)
* OS: Ubuntu 22.04

## Software Dependencies
```
lolremez 0.7
```

### Install 
If you use docker image, you can skip this script.
```bash
sudo bash ./scripts/install.sh
```

### Build
```bash
bash ./scripts/build.sh
```

### Run SLOTHE examples on NAF
The command below runs examples of SLOTHE with results (predicted latency and error) shown in the cmd prompt lines.
```bash
bash ./scripts/run_examples.sh
```

### Basic Description

- `math` folder : includes the C source codes of the NAFs used in our work gathered from the backends used when running PyTorch such as Eigen (which in turn calls glibc) and the auto-generated C codes for backends from Python (e.g., ReLU). We leave references of the glibc codes.
- `results` folder : refers to the folder where the (intermediate) results will be stored once you run the code. The default IRB will be saved as png file (named only with the NAF name), visualizing the IRB structure. Unreacheable, merged, and optim refers to the IRB strcuture images optimized by the CF-Optimizer. The file named with result, since it is the final one, will be saved as `ll` format (LLVM's platform-agnostic assembly language) which is the final result of SLOTHE (the optimized IRB after FBA).
- `scripts`, `src`, and `utils` folder include the running shell, cpp codes and other miscellaneous files used during SLOTHE.
- The intermediate results' `ll` files and other temporary files will be stored in a temporary folder `temp` but they are cleaned (deleted) once you run `clean.sh`.

  A brief description of the flow is as follows (follow the flow with figure 1 in our paper). The NAF C code is read from `math` folder. Running SLOTHE first runs `countcond.cpp` which first assesses the number of conditional paths before running CF-Optimizer. When running CF-Optimizer, codes such as `merge.cpp` and `unreacheable.cpp` are run along with corresponding shell codes to find paths to merge and eliminate. Before beginning FBA, `costestimation.cpp` estimates the costs of each sub-functions to choose the one to execute FBA with. FBA is run with the shell code `FBA.sh`, where you can see that the algorithms from our design section are implemented. It also calls several other files sicu as `opanalyzer.cpp` (which is as the name shows, the Op-Analyzer from our work). PAG is run with `replacediv.cpp` and `replacefunc.cpp` and also running lol-remez which we included in the dependencies when you run shell codes to install them. UDC Tracker is in `TimeEstimation.cpp` and other files.
