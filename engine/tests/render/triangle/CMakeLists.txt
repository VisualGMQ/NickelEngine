aux_source_directory(. SRC)

add_executable(triangle ${SRC} shader.vert shader.frag)
mark_as_gui_test(triangle renderer)

compile_shader(shader.frag frag.spv)
compile_shader(shader.vert vert.spv)