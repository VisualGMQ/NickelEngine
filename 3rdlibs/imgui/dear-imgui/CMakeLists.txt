aux_source_directory(./ imgui_src)

add_library(imgui STATIC)
target_sources(imgui PRIVATE ${imgui_src})

add_subdirectory(backends/sdl2)
add_subdirectory(backends/gl3)
if (TARGET Vulkan)
    add_subdirectory(backends/vk)
    target_link_libraries(imgui PUBLIC imgui_backend_gl3 imgui_backend_vk)
endif()

target_link_libraries(imgui PUBLIC imgui_backend_gl3 imgui_backend_sdl2)
target_include_directories(imgui PUBLIC ./)

