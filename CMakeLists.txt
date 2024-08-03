cmake_minimum_required(VERSION 3.8)
project(AsmVsZombies)

# set global options
add_compile_options(-m32 -std=c++20 -Wall)
add_link_options(-m32)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# build libavz
aux_source_directory(src avz_srcs)
aux_source_directory(src/minhook minhook_srcs)
add_library(avz ${avz_srcs} ${minhook_srcs})
target_compile_options(avz PRIVATE -fexperimental-library)
target_include_directories(avz PRIVATE inc)

# build injector
add_subdirectory(tools/injector)

# 此条命令仅用于作者打包 avz，仅编译 avz 请注释这条命令
add_custom_target(PackageAvz
    COMMAND powershell -c "\
        cd ${PROJECT_SOURCE_DIR}; \
        rm bin/*.dll; \
        Compress-Archive -Path bin/,inc/,src/,metadata.json -Force -DestinationPath release/env2/nightly.zip"
    DEPENDS avz injector
)
