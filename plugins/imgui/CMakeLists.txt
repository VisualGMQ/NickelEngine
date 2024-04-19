add_library(ImGuiPlugin STATIC)
target_sources(ImGuiPlugin PRIVATE imgui_plugin.cpp)
target_link_libraries(ImGuiPlugin PRIVATE Nickel.Video Nickel.Graphics)
target_link_libraries(ImGuiPlugin PUBLIC Nickel.Video imgui ImGuizmo)
target_include_directories(ImGuiPlugin PUBLIC ./)

add_library(Plugin::ImGui ALIAS ImGuiPlugin)