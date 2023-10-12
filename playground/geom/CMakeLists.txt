aux_source_directory(./ geom_playground_src)
add_executable(geom_playground ${geom_playground_src})
target_link_libraries(geom_playground PRIVATE ${engine_name} imgui)

