add_executable(test_smath smath.cpp)
target_link_libraries(test_smath PRIVATE NickelEngine)
mark_as_cli_test(test_smath)

add_executable(test_dmath dmath.cpp)
target_link_libraries(test_dmath PRIVATE NickelEngine)
mark_as_cli_test(test_dmath)
