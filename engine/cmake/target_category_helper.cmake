macro(mark_as_3rdlib target_name)
    if (TARGET ${target_name})
        set_target_properties(${target_name} PROPERTIES FOLDER 3rdlibs)
    endif()
endmacro()

macro(mark_as_gui_test target_name)
    add_test(NAME ${target_name}
            COMMAND ${target_name}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_target_properties(${target_name} PROPERTIES
        FOLDER tests
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    target_link_libraries(${target_name} PRIVATE CommonRenderTestContext Catch2 ${NICKEL_MAIN_ENTRY_NAME})
    set_tests_properties(${target_name} PROPERTIES LABELS "gui")
endmacro()

macro(mark_as_cli_test target_name)
    add_test(NAME ${target_name}
            COMMAND ${target_name}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_target_properties(${target_name} PROPERTIES
        FOLDER tests
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    target_link_libraries(${target_name} PRIVATE Catch2WithMain ${NICKEL_ENGINE_NAME})
    set_tests_properties(${target_name} PROPERTIES LABELS "cli")
endmacro()

