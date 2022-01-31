cmake_minimum_required(VERSION 3.5)
project(bench-sleigh)

add_subdirectory("../../libs/sleigh" "sleigh")

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
