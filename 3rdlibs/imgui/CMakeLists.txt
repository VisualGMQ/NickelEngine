aux_source_directory(./ imgui_src)
aux_source_directory(./backends imgui_backends)

add_library(imgui STATIC ${imgui_src} ${imgui_backends})
target_include_directories(imgui PUBLIC ./ backends)
target_link_libraries(imgui PUBLIC SDL2)
