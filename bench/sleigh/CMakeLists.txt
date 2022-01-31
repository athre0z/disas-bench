cmake_minimum_required(VERSION 3.5)
project(bench-sleigh)

add_subdirectory("../../libs/sleigh" "sleigh")

# The load_bin helpers are written in C and can't be included in the C++ benchmark code
#
# To use the helpers, we've created a C library called load_bin and wrapped it with a C++ compatible
# header (`load_bin.h`)
set("load-bin-files" "../load_bin.inc")
add_library("load-bin" ${load-bin-files})
set_source_files_properties(${load-bin-files} PROPERTIES LANGUAGE C COMPILE_OPTIONS -xc)

add_executable("bench-sleigh-fmt" "main.cpp")
target_compile_features(bench-sleigh-fmt PRIVATE cxx_std_17)
target_link_libraries(
  "bench-sleigh-fmt"
  PRIVATE
  "load-bin"
  "sleigh::decomp"
  "sleigh::sla"
  "sleigh::support")
