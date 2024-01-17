aux_source_directory(. imguizmo_src)
add_library(ImGuizmo STATIC ${imguizmo_src})
target_include_directories(ImGuizmo PUBLIC .)
target_link_libraries(ImGuizmo PUBLIC imgui)
