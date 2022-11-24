# CMakeLists
project(AsmVsZombies)
cmake_minimum_required (VERSION 3.0)

set(CMAKE_CXX_FLAGS "-m32 -static -std=c++20 -Wall")

# build libavz.a
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 
include_directories(./inc)
aux_source_directory(./src avz_srcs)
add_library(avz ${avz_srcs})

# build injector.exe
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
aux_source_directory(./tools/injector/src injector_srcs)
add_executable(injector ${injector_srcs})