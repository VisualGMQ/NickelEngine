aux_source_directory(. SRC)

file(GLOB FILES ./*)

add_executable(texture_rect ${SRC} ${FILES})
mark_as_gui_test(texture_rect renderer)

compile_shader(shader.frag frag.spv)
compile_shader(shader.vert vert.spv)