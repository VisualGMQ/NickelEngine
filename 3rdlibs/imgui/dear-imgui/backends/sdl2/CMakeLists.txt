add_library(imgui_backend_sdl2 STATIC imgui_impl_sdl2.cpp)
target_include_directories(imgui_backend_sdl2 PUBLIC .)
target_link_libraries(imgui_backend_sdl2 PUBLIC SDL2)
target_include_directories(imgui_backend_sdl2 PRIVATE ../../)