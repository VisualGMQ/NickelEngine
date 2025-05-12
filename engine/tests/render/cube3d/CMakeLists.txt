aux_source_directory(. SRC)

file(GLOB FILES ./*)

add_executable(cube3d ${SRC} ${FILES})
mark_as_gui_test(cube3d renderer)

compile_shader(shader.frag frag.spv)
compile_shader(shader.vert vert.spv)