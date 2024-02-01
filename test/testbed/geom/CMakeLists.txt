aux_source_directory(./ testbed_src)
add_executable(geom_testbed ${testbed_src})
target_link_libraries(geom_testbed PRIVATE Nickel.Runtime Plugin::ImGui)
CopyDLL(geom_testbed)
