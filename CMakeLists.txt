# CMakeLists
project(AsmVsZombies)
cmake_minimum_required (VERSION 3.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-m32 -static -std=c++20 -Wall")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# build libavz.a
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 
include_directories(./inc)
aux_source_directory(./src avz_srcs)
add_library(avz ${avz_srcs})


# 此条命令仅用于作者打包 avz，仅编译 avz 请注释这条命令
add_custom_command( 
    TARGET avz POST_BUILD
    COMMAND python ${PROJECT_SOURCE_DIR}/tools/copy.py ${PROJECT_SOURCE_DIR}/ D:/AsmVsZombies/
)

# build injector.exe
add_subdirectory(tools/injector)

# build debug dll
# 这条命令仅用来作者自己调试使用，仅编译 avz 请注释下面这条命令
add_subdirectory(debug)