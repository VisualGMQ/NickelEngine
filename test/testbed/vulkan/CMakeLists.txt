aux_source_directory(./ vulkan_test_src)
add_executable(vulkan_testbed ${vulkan_test_src})
target_link_libraries(vulkan_testbed PRIVATE Nickel)
CopyDLL(vulkan_testbed)

