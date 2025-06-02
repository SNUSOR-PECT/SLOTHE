#!/bin/bash
# This script is written by GenAI and modified by the authors

set -e

echo "[*] Installing dependencies for LLVM-based C++17 project"

# --------- System Update ---------
sudo apt update && sudo apt upgrade -y

# --------- Install Prerequisites ---------
sudo apt install -y git wget curl build-essential software-properties-common gnupg lsb-release automake autoconf libtool pkg-config cmake graphviz

# --------- Install LLVM 16 ---------
echo "[*] Installing LLVM 16..."
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 16
rm llvm.sh

# --------- Install GCC 11.4 ---------
echo "[*] Installing GCC 11.4..."
sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
sudo apt update
sudo apt install -y gcc-11 g++-11

# Set GCC 11 as default
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 110

# --------- Verify GCC version ---------
gcc --version
g++ --version

# --------- Install CMake >= 3.20 ---------
echo "[*] Installing CMake 3.22 (default pick)..."
CMAKE_VER=3.22.1
cd /tmp
wget https://github.com/Kitware/CMake/releases/download/v$CMAKE_VER/cmake-$CMAKE_VER-linux-x86_64.sh
chmod +x cmake-$CMAKE_VER-linux-x86_64.sh
sudo ./cmake-$CMAKE_VER-linux-x86_64.sh --skip-license
rm cmake-$CMAKE_VER-linux-x86_64.sh

# --------- Install lolremez ---------
git clone https://github.com/samhocevar/lolremez.git
cd lolremez
git submodule update --init --recursive
./bootstrap
./configure
make install
cd ..

# --------- Final Check ---------
echo "[*] Installed versions:"
echo -n "GCC: "; gcc --version | head -n1
echo -n "G++: "; g++ --version | head -n1
echo -n "CMake: "; cmake --version | head -n1
echo -n "Clang: "; clang-16 --version | head -n1
echo -n "lolremez: "; lolremez --version | head -n1

echo "[*] All dependencies installed successfully."
