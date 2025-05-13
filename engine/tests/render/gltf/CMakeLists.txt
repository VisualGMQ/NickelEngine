aux_source_directory(. SRC)

add_executable(gltf ${SRC})
mark_as_gui_test(gltf renderer)