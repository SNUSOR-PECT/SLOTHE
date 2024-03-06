Custom Pass based on llvm-tutor examples
=========


Tracking path of each BasicBlock - based on **LLVM 16**

### Build (llvm16 is installed at /usr/local)
```bash
cmake -S . -B build -DLT_LLVM_INSTALL_DIR=/usr/local
cmake --build build
```
or run `build.sh`

### Run
```bash
/usr/local/bin/opt -load-pass-plugin ./build/lib/libBasicTrace.so -passes=basic-trace -disable-output _tanh.ll
```
or run `run.sh`