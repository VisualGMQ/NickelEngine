macro(AddConsoleTest name)
    add_executable(${name} ${name}.cpp)    
    target_include_directories(${name} PRIVATE ./ ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE Nickel.Common)
    add_test(NAME ${name}
             COMMAND $<TARGET_FILE:${name}>)
    set_tests_properties(${name} PROPERTIES LABELS "console")
endmacro()

macro(AddVisualableTest name)
    add_executable(${name} ${name}.cpp)    
    target_include_directories(${name} PRIVATE ./ ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE Nickel.Runtime)
    add_test(NAME ${name}
             COMMAND $<TARGET_FILE:${name}>)
    set_tests_properties(${name} PROPERTIES LABELS "visualable")
    CopyDLL(${name})
endmacro()

AddConsoleTest(geom)
AddConsoleTest(cgmath)
AddConsoleTest(tweeny)
AddConsoleTest(csv_iterator)

# AddVisualableTest(gjk)
# AddVisualableTest(script)

add_subdirectory(testbed)
add_subdirectory(graphics)