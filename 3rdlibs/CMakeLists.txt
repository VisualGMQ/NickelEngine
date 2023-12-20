add_subdirectory(sdl2)
add_subdirectory(gecs)
add_subdirectory(glad)
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