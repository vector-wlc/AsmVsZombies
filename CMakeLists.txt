# CMakeLists
cmake_minimum_required (VERSION 3.8)
project(AsmVsZombies)

set(CMAKE_CXX_FLAGS "-m32 -static -std=c++2b -Wall -fexperimental-library")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# build libavz.a
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
include_directories(./inc)
aux_source_directory(./src avz_srcs)
add_library(avz ${avz_srcs})

# build injector.exe
add_subdirectory(tools/injector)

# 此条命令仅用于作者打包 avz，仅编译 avz 请注释这条命令
add_custom_target(PackageAvz
    COMMAND powershell -c "\
        cd ${PROJECT_SOURCE_DIR}; \
        rm bin/*.dll; \
        Compress-Archive -Path bin/,inc/,src/,metadata.json -Force -DestinationPath release/env2/nightly.zip"
    DEPENDS avz injector
)
