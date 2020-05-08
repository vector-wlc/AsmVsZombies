# CMakeLists

cmake_minimum_required (VERSION 3.0)

add_definitions(-std=c++17)

set(CMAKE_CXX_COMPILER "g++")

# build libpvz.dll
include_directories(./AvZ/src)
aux_source_directory(./AvZ/src srcs)
aux_source_directory(./script/200508 srcs)
add_library(pvz SHARED ${srcs})

# build injector.exe
# include_directories(./AvZ/injector/inc)
# aux_source_directory(./AvZ/injector/src injector_srcs)
# add_executable(injector ${injector_srcs})