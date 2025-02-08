file(GLOB_RECURSE IMGUI_FILES ./*.h ./*.cpp)

add_library(imgui STATIC )
target_sources(imgui PRIVATE ${IMGUI_FILES})
target_include_directories(imgui PUBLIC .)
target_compile_features(imgui PRIVATE cxx_std_17)
target_link_libraries(imgui PRIVATE SDL3::SDL3 volk::volk)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_VULKAN_USE_VOLK)

compile_shader(backends/vulkan/glsl_shader.vert glsl_shader.vert.u32)
compile_shader(backends/vulkan/glsl_shader.frag glsl_shader.frag.u32)