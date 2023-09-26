
# build injector.exe
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
aux_source_directory(./src srcs)
add_executable(injector ${srcs})

# 此条命令仅用于作者压缩 injector.exe，仅编译 injector 请注释这条命令
add_custom_command( 
    TARGET injector POST_BUILD
    COMMAND d:/upx/upx.exe ${PROJECT_SOURCE_DIR}/bin/injector.exe
)