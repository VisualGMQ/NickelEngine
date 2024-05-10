cmake_minimum_required(VERSION 3.21)
project(NickelEngine
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (ANDROID)
    message(STATUS "build platform: Android")
elseif(EMSCRIPTEN)
    message(STATUS "build platform: Emscripten")
elseif(WIN32)
    message(STATUS "build platform: Windows")
elseif(APPLE)
    message(STATUS "build platform: Apple")
elseif(LINUX)
    message(STATUS "build platform: Linux")
endif()

# some cmake utilities
option(NICKEL_COPYDLL "auto copy needed dll to output dir" OFF)
include(cmake/copy_dll.cmake)

if (EMSCRIPTEN)
    add_compile_options(-sUSE_SDL=2  -sSUPPORT_LONGJMP=wasm)
# -fwasm-exceptions
    add_link_options(-sFULL_ES3)
endif()

option(NICKEL_RHI_ENABLE_VULKAN "enable vulkan" ON)
add_subdirectory(3rdlibs)
add_subdirectory(nickel)
add_subdirectory(plugins)

option(NICKEL_BUILD_EDITOR "build editor" ON)
if (NICKEL_BUILD_EDITOR)
    add_subdirectory(editor)
endif()

option(NICKEL_ENABLE_BENCHMARK "build benchmark" OFF)
if (NICKEL_ENABLE_BENCHMARK OR PROJECT_IS_TOP_LEVEL)
    add_subdirectory(benchmark)
endif()

option(NICKEL_BUILD_UNITTEST "build unit test" OFF)
if (NICKEL_BUILD_UNITTEST OR PROJECT_IS_TOP_LEVEL)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()