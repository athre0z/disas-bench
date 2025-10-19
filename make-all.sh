#!/usr/bin/env bash
set -e

if [[ `uname -s` == "MINGW"* ]]; then
    is_windows=y
elif [[ `uname -s` == "Darwin" ]]; then
    is_mac=y
else
    is_linux=y
fi

if [[ "$is_windows" == "y" ]]; then
    make='nmake -f Makefile.win'
else
    make=make
fi

python=python

if [[ "$is_windows" == "y" ]]; then
    if ! link.exe --help 2>&1 | grep -i Microsoft > /dev/null; then
        export PATH=$(echo "$PATH" | tr ':' '\n' | grep HostX64):$PATH
    fi
    if ! link.exe --help 2>&1 | grep -i Microsoft > /dev/null; then
        echo "[-] Couldn't find VS 64-bit link.exe. Make sure it's in your path!"
        exit 1
    fi
else
    if [[ ! `which cmake` ]]; then
        echo "[-] Unable to find CMake, is it installed?" 1>&2
        exit 1
    fi
    if [[ $(which python3) ]]; then
        python=python3
    elif [[ ! `which $python` ]]; then
        echo "[-] Unable to find Python, is it installed?" 1>&2
        exit 1
    fi
    if [[ ! `which cargo` ]]; then
        echo "[-] Unable to find cargo (Rust), is it installed?" 1>&2
        exit 1
    fi
fi

# Build Capstone

echo "[*] Building Capstone ..."
cd libs/capstone
if [[ "$is_windows" == "y" ]]; then
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DCAPSTONE_BUILD_STATIC_RUNTIME=ON -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_X86_SUPPORT=ON -G "NMake Makefiles" ..
    nmake
    cd ../../..
else
    export CAPSTONE_ARCHS="x86"
    $make
    cd ../..
fi

# Build Intel XED

echo "[*] Building Intel XED ..."
cd libs/intelxed
$python mfile.py --opt=3
cd ../..

# Build DiStorm

echo "[*] Building DiStorm ..."
if [[ "$is_mac" == "y" ]]; then 
    distorm_subdir=mac
    distorm_bin=libdistorm3.dylib
elif [[ "$is_windows" == "y" ]]; then
    distorm_subdir=win32
else
    distorm_subdir=linux
    distorm_bin=libdistorm3.so.3.4.0
fi
cd libs/distorm/make/${distorm_subdir}
if [[ "$is_windows" == "y" ]]; then
    msbuild.exe cdistorm.vcxproj -p:Configuration=clib -p:Platform=x64
else
    $make
    cp ${distorm_bin} ../../../../bench/distorm/
fi
cd ../../../..
if [[ "$is_linux" == "y" ]]; then
    cd bench/distorm/
    ln -s libdistorm3.so.3.4.0 libdistorm3.so.3 || true
    cd ../..
fi

# Build bddisasm

echo "[*] Building bddisasm ..."
cd libs/bddisasm
if [[ "$is_windows" == "y" ]]; then
    msbuild.exe bddisasm/bddisasm.vcxproj -p:Configuration=Release -p:Platform=x64
else
    $make
fi
cd ../..

# Build benchmark tools

echo "[*] Building Capstone benchmark ..."
cd bench/cs
$make
cd ../..

echo "[*] Building Zydis benchmark ..."
cd bench/zydis
cmake -DCMAKE_BUILD_TYPE=Release .
if [[ "$is_windows" == "y" ]]; then
    msbuild.exe bench-zydis.sln -p:Configuration=Release -p:Platform=x64
    cp Release/*.exe .
    cp zydis/Release/Zydis.dll .
else
    $make
fi
cd ../..

echo "[*] Building Intel XED benchmark ..."
cd bench/xed
$make
cd ../..

echo "[*] Building DiStorm benchmark ..."
cd bench/distorm
$make
cd ../..

echo "[*] Building iced-x86 benchmark ..."
cd bench/iced-x86
$make
cd ../..

echo "[*] Building bddisasm benchmark ..."
cd bench/bddisasm
$make
cd ../..

echo "[*] Building yaxpeax-x86 benchmark ..."
cd bench/yaxpeax
$make
cd ../..

echo "[*] Building yaxpeax-arm benchmark ..."
cd bench/armv7/yaxpeax-arm
$make
cd ../../..

echo "[*] Building sleigh benchmark ..."
cd bench/sleigh
cmake -DCMAKE_BUILD_TYPE=Release .
if [[ "$is_windows" == "y" ]]; then
    msbuild.exe bench-sleigh.sln -p:Configuration=Release -p:Platform=x64
    cp Release/*.exe .
    cp sleigh/Release/Sleigh.dll .
else
    $make -j4
fi
cd ../..

echo "[+] All done!"
