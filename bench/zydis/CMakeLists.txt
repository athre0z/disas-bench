cmake_minimum_required(VERSION 3.5)
project(bench-zydis)

set(ZYDIS_BUILD_SHARED_LIB OFF CACHE BOOL "")
set(ZYDIS_BUILD_EXAMPLES OFF CACHE BOOL "")
set(ZYDIS_BUILD_TOOLS OFF CACHE BOOL "")
add_subdirectory("../../libs/zydis" "zydis")

add_executable("bench-zydis-full-fmt" "main.c")
target_link_libraries("bench-zydis-full-fmt" "Zycore" "Zydis")

add_executable("bench-zydis-full-no-fmt" "main.c")
target_compile_definitions(
    "bench-zydis-full-no-fmt"
    PUBLIC
    "DISAS_BENCH_NO_FORMAT")
target_link_libraries("bench-zydis-full-no-fmt" "Zycore" "Zydis")

add_executable("bench-zydis-min-no-fmt" "main.c")
target_compile_definitions(
    "bench-zydis-min-no-fmt"
    PUBLIC
    "DISAS_BENCH_NO_FORMAT"
    "DISAS_BENCH_DECODE_MINIMAL")
target_link_libraries("bench-zydis-min-no-fmt" "Zycore" "Zydis")
