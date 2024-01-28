macro(AddVulkanTest test_name)
    add_executable(${test_name} main.cpp)
    target_link_libraries(${test_name} PRIVATE nickel)
    CopyDLL(${test_name})
endmacro(AddVulkanTest)

add_subdirectory(01clearscreen)
add_subdirectory(02triangle)
add_subdirectory(03vertex_buffer)