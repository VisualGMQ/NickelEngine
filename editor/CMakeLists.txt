aux_source_directory(src editor_src)

file(GLOB_RECURSE header_files ./src/*.hpp)

add_executable(editor ${editor_src} ${header_files})
target_link_libraries(editor PRIVATE Nickel Plugin::ImGui efsw)
CopyDLL(editor)