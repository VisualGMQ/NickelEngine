aux_source_directory(. SRC)

add_executable(test_screen_clear ${SRC})
target_link_libraries(test_screen_clear PRIVATE ${NICKEL_ENGINE_NAME} ${NICKEL_MAIN_ENTRY_NAME})
mark_as_gui_test(test_screen_clear)
