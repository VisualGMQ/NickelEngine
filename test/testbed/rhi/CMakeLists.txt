macro(AddRHITest test_name)
    add_executable(${test_name} main.cpp)
    target_link_libraries(${test_name} PRIVATE Nickel.Runtime Nickel.Graphics)
    CopyDLL(${test_name})
endmacro(AddRHITest)

add_subdirectory(01triangle)