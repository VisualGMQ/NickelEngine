aux_source_directory(./ testbed_src)
add_executable(physics_testbed ${testbed_src})
target_link_libraries(physics_testbed PRIVATE Nickel.Runtime imgui Plugin::ImGui)
CopyDLL(physics_testbed)

