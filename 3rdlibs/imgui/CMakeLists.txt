aux_source_directory(./ imgui_src)
set(imgui_glfw_backend ./backends/imgui_impl_glfw.cpp)
set(imgui_opengl3_backend ./backends/imgui_impl_opengl3.cpp)

add_library(imgui STATIC ${imgui_src} ${imgui_glfw_backend} ${imgui_opengl3_backend})
target_include_directories(imgui PUBLIC ./ backends)
target_link_libraries(imgui PUBLIC glfw)
