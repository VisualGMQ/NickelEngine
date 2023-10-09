aux_source_directory(src editor_src)

add_executable(editor ${editor_src})
target_link_libraries(editor PRIVATE ${engine_name} imgui)
target_precompile_headers(editor REUSE_FROM ${engine_name})
