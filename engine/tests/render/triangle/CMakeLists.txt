aux_source_directory(. SRC)

add_executable(test_triangle ${SRC} shader.vert shader.frag)
mark_as_gui_test(test_triangle)
