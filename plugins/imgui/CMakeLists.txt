add_library(ImGuiPlugin STATIC)
target_sources(ImGuiPlugin PRIVATE imgui_plugin.cpp)
target_link_libraries(ImGuiPlugin PUBLIC Nickel.Video Nickel.Graphics imgui ImGuizmo SDL2)
target_include_directories(ImGuiPlugin PUBLIC ./)

add_library(Plugin::ImGui ALIAS ImGuiPlugin)