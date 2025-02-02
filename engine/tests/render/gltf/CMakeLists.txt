aux_source_directory(. SRC)

add_executable(test_gltf ${SRC})
mark_as_gui_test(test_gltf)