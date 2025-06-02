#!/bin/bash

cmake -S . -B build -DLT_LLVM_INSTALL_DIR=/usr/lib/llvm-16
cmake --build build --parallel