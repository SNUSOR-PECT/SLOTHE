SLOTHE : Lazy Approximation of Non-Arithmetic Neural Network Functions over Encrypted Data
=========

based on **LLVM 16**

### Build (llvm16 is installed at /usr/local)
```bash
cmake -S . -B build -DLT_LLVM_INSTALL_DIR=/usr/local
cmake --build build
```
or run `build.sh`

### Run
Now we provide two options:
1. basic-trace
2. remove-specials (requires `simplify-cfg` pass) and draw new CFG (located in assets/)

```bash
./run.sh 0 # to run basic-trace
./run.sh 1 # to run remove-specials and simplify-cfg
```
