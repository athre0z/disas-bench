#!/usr/bin/env bash

if [[ ! `which cmake` ]]; then
    echo "[-] Unable to find CMake, is it installed?" 1>&2
    exit 1
fi
if [[ ! `which python` ]]; then
    echo "[-] Unable to find Python, is it installed?" 1>&2
    exit 1
fi

# Build Capstone

echo "[*] Building Capstone ..."
cd libs/capstone
export CAPSTONE_ARCHS="x86"
make -j12  || exit 1
cd ../..

# Build Intel XED

echo "[*] Building Intel XED ..."
cd libs/intelxed
python mfile.py --opt=3
cd ../..

# Build DiStorm

echo "[*] Building DiStorm ..."
if [[ `uname -s` == "Darwin" ]]; then 
    distorm_subdir=mac
    distorm_bin=libdistorm3.dylib
else
    distorm_subdir=linux
    distorm_bin=libdistorm3.so
fi
cd libs/distorm/make/${distorm_subdir}
make || exit 1
cp ${distorm_bin} ../../../../bench-distorm/
cd ../../../..

# Build benchmark tools

echo "[*] Building Capstone benchmark ..."
cd bench/cs
make || exit 1
cd ../..

echo "[*] Building Zydis benchmark ..."
cd bench/zydis
cmake -DCMAKE_BUILD_TYPE=Release . || exit 1
make || exit 1
cd ../..

echo "[*] Building Intel XED benchmark ..."
cd bench/xed
make || exit 1
cd ../..

echo "[*] Building DiStorm benchmark ..."
cd bench/distorm
make || exit 1
cd ../..

echo "[+] All done!"

