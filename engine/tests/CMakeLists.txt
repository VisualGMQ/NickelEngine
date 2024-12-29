macro(mark_as_test target_name)
    add_test(NAME ${target_name}
            COMMAND ${target_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_target_properties(${target_name} PROPERTIES FOLDER tests)
endmacro()

add_subdirectory(sandbox)

