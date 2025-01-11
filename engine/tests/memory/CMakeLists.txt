aux_source_directory(. SRC)

add_executable(test_memory ${SRC})
mark_as_cli_test(test_memory)
