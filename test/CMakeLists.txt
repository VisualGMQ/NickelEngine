macro(AddTest name)
    add_executable(${name} ${name}.cpp)    
    target_include_directories(${name} PRIVATE ./ ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE ${engine_name})
    add_test(NAME ${name}
             COMMAND $<TARGET_FILE:${name}>)
endmacro()

AddTest(tweeny)
