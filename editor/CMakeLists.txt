aux_source_directory(src editor_src)

file(GLOB_RECURSE header_files ./src/*.hpp)

add_executable(editor ${editor_src} ${header_files})
target_link_libraries(editor PRIVATE Nickel.Runtime Plugin::ImGui efsw SDL2)
CopyDLL(editor)

include(InstallRequiredSystemLibraries)
install(TARGETS editor
    DESTINATION ${CMAKE_SOURCE_DIR}/install/editor
)