aux_source_directory(src editor_src)

add_executable(editor ${editor_src})
target_link_libraries(editor PRIVATE Nickel Plugin::ImGui)
CopyDLL(editor)