aux_source_directory(. SRC)

file(GLOB FILES ./*)

add_executable(test_texture_rect ${SRC} ${FILES})
mark_as_gui_test(test_texture_rect)
