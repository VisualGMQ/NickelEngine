add_library(ImGuiPlugin STATIC imgui_plugin.cpp)
target_link_libraries(ImGuiPlugin PUBLIC Nickel ImGuizmo)
target_include_directories(ImGuiPlugin PUBLIC ./)

add_library(Plugin::ImGui ALIAS ImGuiPlugin)