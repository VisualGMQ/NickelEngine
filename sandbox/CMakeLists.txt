aux_source_directory(./ sandbox_src)
add_executable(sandbox ${sandbox_src})
target_link_libraries(sandbox PRIVATE Nickel)
CopyDLL(sandbox)