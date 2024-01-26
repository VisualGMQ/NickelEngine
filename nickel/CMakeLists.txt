aux_source_directory(src/core nickel_core_src)
aux_source_directory(src nickel_src)
aux_source_directory(src/window nickel_src)
aux_source_directory(src/renderer nickel_src)
aux_source_directory(src/input nickel_src)
aux_source_directory(src/misc nickel_src)
aux_source_directory(src/anim nickel_src)
aux_source_directory(src/refl nickel_src)
aux_source_directory(src/physics nickel_src)
aux_source_directory(src/ui nickel_src)
aux_source_directory(src/audio nickel_src)
aux_source_directory(src/script nickel_src)

file(GLOB_RECURSE nickel_header_files ./include/*.hpp)
file(GLOB shader_files ./shader/*.shader)

# make IDE more clearer
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "include" FILES ${nickel_header_files})
source_group(shader FILES ${shader_files})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "src" FILES ${nickel_src})

add_library(nickel_core STATIC)
target_sources(nickel_core PRIVATE ${nickel_core_src})
target_include_directories(nickel_core PUBLIC ./include)
target_compile_features(nickel_core PUBLIC cxx_std_17)
target_link_libraries(nickel_core PUBLIC SDL2 glad stb_image toml++ debugbreak mirrow gecs miniaudio nameof freetype lunasvg rapidxml)
target_link_libraries(nickel_core PUBLIC "$<$<CONFIG:Debug>:easy_profiler>")
target_compile_definitions(nickel_core PUBLIC "$<$<CONFIG:Debug>:NICKEL_ENABLE_PROFILE>")

target_compile_definitions(nickel_core PUBLIC _CRT_SECURE_NO_WARNINGS)

add_library(nickel STATIC)
target_sources(nickel PRIVATE ${nickel_src})
target_link_libraries(nickel PUBLIC nickel_core)

add_library(Nickel::Core ALIAS nickel_core)
add_library(Nickel ALIAS nickel)
