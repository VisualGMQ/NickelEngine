aux_source_directory(src glad_src)
add_library(glad STATIC ${glad_src})
target_include_directories(glad PUBLIC include)