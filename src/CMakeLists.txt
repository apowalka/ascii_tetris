cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# third party
#include_directories(/home/art/Documents/benchmark/include)
#find_library(BENCH_LIB benchmark /home/art/Documents/benchmark/build/src)
#find_library(BENCH_M_LIB benchmark_main /home/art/Documents/benchmark/build/src)
#message(${BENCH_LIB})
#message(${BENCH_M_LIB})

project(ascii_tetris)
set(tetris_sources main.cpp Grid.cpp Shape.h Utility.h)

add_executable(tetris ${tetris_sources})
#target_link_libraries(tetris ${BENCH_LIB} ${BENCH_M_LIB})
