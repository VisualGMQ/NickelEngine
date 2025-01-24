aux_source_directory(. SRC)

file(GLOB FILES ./*)

add_executable(test_skybox ${SRC} ${FILES})
mark_as_gui_test(test_skybox)

compile_shader(shader.frag frag.spv)
compile_shader(shader.vert vert.spv)