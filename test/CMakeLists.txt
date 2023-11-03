macro(AddConsoleTest name)
    add_executable(${name} ${name}.cpp)    
    target_include_directories(${name} PRIVATE ./ ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE Nickel::Core)
    add_test(NAME ${name}
             COMMAND $<TARGET_FILE:${name}>)
    set_tests_properties(${name} PROPERTIES LABELS "console")
    CopyDLL(${name})
endmacro()

macro(AddVisualableTest name)
    add_executable(${name} ${name}.cpp)    
    target_include_directories(${name} PRIVATE ./ ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE Nickel)
    add_test(NAME ${name}
             COMMAND $<TARGET_FILE:${name}>)
    set_tests_properties(${name} PROPERTIES LABELS "visualable")
    CopyDLL(${name})
endmacro()

# AddConsoleTest(tweeny)
AddConsoleTest(geom)
AddConsoleTest(cgmath)

AddVisualableTest(gjk)

add_subdirectory(testbed)