# build injector.exe
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
aux_source_directory(./src srcs)
add_executable(injector ${srcs})
