aux_source_directory(. SRC)

add_executable(test_rectangle ${SRC} shader.vert shader.frag)
mark_as_gui_test(test_rectangle)

compile_shader(shader.vert vert.spv)
compile_shader(shader.frag frag.spv)