aux_source_directory(src injector_srcs)
add_executable(injector ${injector_srcs})
target_link_options(injector PRIVATE -static)
