# CMakeLists

cmake_minimum_required (VERSION 3.0)

add_definitions(-std=c++17)

set(CMAKE_CXX_COMPILER "g++")

# build libavz.a
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 
include_directories(./inc)
aux_source_directory(./src avz_srcs)
add_library(avz ${avz_srcs})

# build injector.exe
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
include_directories(./injector/inc)
aux_source_directory(./injector/src injector_srcs)
add_executable(injector ${injector_srcs})
