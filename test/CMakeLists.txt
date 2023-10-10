macro(AddConsoleTest name)
    add_executable(${name} ${name}.cpp)    
    target_include_directories(${name} PRIVATE ./ ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE ${engine_name})
    add_test(NAME ${name}
             COMMAND $<TARGET_FILE:${name}>)
    set_tests_properties(${name} PROPERTIES LABELS "console")
endmacro()

macro(AddVisualableTest name)
    add_executable(${name} ${name}.cpp)    
    target_include_directories(${name} PRIVATE ./ ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE ${engine_name})
    add_test(NAME ${name}
             COMMAND $<TARGET_FILE:${name}>)
    set_tests_properties(${name} PROPERTIES LABELS "visualable")
endmacro()

AddConsoleTest(tweeny)
AddConsoleTest(geom)
AddConsoleTest(cgmath)

AddVisualableTest(gjk)