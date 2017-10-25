#!/usr/bin/env bash

(cd capstone; make clean)
(cd zydis; make clean)
(cd bench-cs; make clean)
(cd bench-zydis; rm -rf zydis cmake Makefile CMakeCache.txt CMakeFiles cmake_install.cmake bench-zydis*)
(cd bench-xed; make clean)
(cd bench-distorm; make clean)
(cd intelxed; python mfile.py clean)
rm bench-distorm/libdistorm*
rm distorm/make/mac/libdistorm*
rm distorm/make/linux/libdistorm*
rm distorm/src/*.o
rm -rf distorm/python/macosx-x86
rm bench*.png
