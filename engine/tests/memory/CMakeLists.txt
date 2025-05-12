aux_source_directory(. SRC)

add_executable(memory ${SRC})
mark_as_cli_test(memory memory)
