aux_source_directory(src injector_srcs)
add_executable(injector ${injector_srcs})
target_compile_options(injector PRIVATE -std=c++20 -Wall)
target_link_options(injector PRIVATE -static)
