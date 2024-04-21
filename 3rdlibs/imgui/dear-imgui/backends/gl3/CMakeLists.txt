add_library(imgui_backend_gl3 STATIC imgui_impl_opengl3.cpp)
target_link_libraries(imgui_backend_gl3 PUBLIC glad)
target_include_directories(imgui_backend_gl3 PUBLIC .)
target_include_directories(imgui_backend_gl3 PRIVATE ../../)