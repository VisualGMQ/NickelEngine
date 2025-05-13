aux_source_directory(. SRC)

file(GLOB FILES ./*)

add_executable(colorful_rectangle ${SRC} ${FILES})
mark_as_gui_test(colorful_rectangle renderer)

compile_shader(shader.frag frag.spv)
compile_shader(shader.vert vert.spv)