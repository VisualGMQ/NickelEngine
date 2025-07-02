aux_source_directory(. SRC)

add_executable(handle ${SRC})
mark_as_cli_test(handle handle)
