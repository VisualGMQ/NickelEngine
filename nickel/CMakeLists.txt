aux_source_directory(src nickel_src)
aux_source_directory(src/window nickel_src)
aux_source_directory(src/renderer nickel_src)
aux_source_directory(src/input nickel_src)
aux_source_directory(src/misc nickel_src)
aux_source_directory(src/anim nickel_src)
aux_source_directory(src/refl nickel_src)
aux_source_directory(src/core nickel_src)

file(GLOB_RECURSE nickel_header_files ./include/*.hpp)
file(GLOB shader_files ./shader/*.shader)

# make IDE more clearer
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "include" FILES ${nickel_header_files})
source_group(shader FILES ${shader_files})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "src" FILES ${nickel_src})

add_library(${engine_name} STATIC)
target_sources(${engine_name} PRIVATE ${nickel_src} ${nickel_header_files} ${shader_files})
target_compile_features(${engine_name} PUBLIC cxx_std_17)
target_link_libraries(${engine_name} PUBLIC SDL2 glad gecs stb_image toml++ debugbreak mirrow miniaudio nameof)
target_include_directories(${engine_name} PUBLIC ./include)
target_precompile_headers(${engine_name} PRIVATE include/pch.hpp)
