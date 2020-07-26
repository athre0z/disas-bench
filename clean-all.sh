#!/usr/bin/env bash

(cd libs/capstone; make clean)
(cd libs/zydis; make clean)
(cd libs/intelxed; python mfile.py clean)
(cd libs/bddisasm; make clean)

(cd bench/cs; make clean)
(cd bench/zydis; rm -rf zydis cmake Makefile CMakeCache.txt CMakeFiles cmake_install.cmake bench-zydis*)
(cd bench/xed; make clean)
(cd bench/distorm; make clean)
(cd bench/iced-x86; make clean)
(cd bench/bddisasm; make clean)
rm bench/distorm/libdistorm*

rm libs/distorm/make/mac/libdistorm*
rm libs/distorm/make/linux/libdistorm*
rm libs/distorm/src/*.o
rm -rf libs/distorm/python/macosx-x86
rm bench*.png
