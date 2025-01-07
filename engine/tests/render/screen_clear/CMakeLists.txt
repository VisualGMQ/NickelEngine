aux_source_directory(. SRC)

add_executable(test_screen_clear ${SRC})
mark_as_gui_test(test_screen_clear)
