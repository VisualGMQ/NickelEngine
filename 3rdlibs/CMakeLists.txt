if (NOT EMSCRIPTEN)
    add_subdirectory(sdl2)
endif()

if (NICKEL_RHI_ENABLE_VULKAN AND NOT EMSCRIPTEN)
    add_subdirectory(vulkan)
endif()

add_subdirectory(gecs)

if (NOT EMSCRIPTEN)
    add_subdirectory(glad)
endif()

add_subdirectory(imgui)
add_subdirectory(stb_image)
add_subdirectory(tomlplusplus)
add_subdirectory(debugbreak)
add_subdirectory(rapidxml)

set(MIRROW_ENABLE_TOML++ ON)
add_subdirectory(mirrow)

add_subdirectory(miniaudio)
add_subdirectory(nameof)
add_subdirectory(nanobench)

set(FT_ENABLE_ERROR_STRINGS ON)
add_subdirectory(freetype2)

add_subdirectory(lunasvg)

if (NOT EMSCRIPTEN)
    add_subdirectory(efsw)
endif()


if (NOT EMSCRIPTEN)
    set(EASY_PROFILER_NO_GUI ON)   
    add_subdirectory(easy_profiler)
endif()

set(TINYGLTF_HEADER_ONLY ON)
set(TINYGLTF_BUILD_GL_EXAMPLES OFF)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF)
set(TINYGLTF_BUILD_VALIDATOR_EXAMPLE OFF)
set(TINYGLTF_INSTALL OFF)
add_subdirectory(tinygltf)

add_subdirectory(luau)
add_subdirectory(LuaBridge3)