aux_source_directory(. SRC)

file(GLOB FILES ./*)

add_executable(test_colorful_rectangle2 ${SRC} ${FILES})
mark_as_gui_test(test_colorful_rectangle2)

compile_shader(shader.frag frag.spv)
compile_shader(shader.vert vert.spv)