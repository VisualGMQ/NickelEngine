cmake_minimum_required(VERSION 3.20)

project(NickelEngine
    VERSION 0.1.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(NICKEL_TESTS "build tests" OFF)

set(CMAKE_VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(NICKEL_MAIN_ENTRY_NAME NickelEngine-Main)
set(NICKEL_ENGINE_NAME NickelEngine)

find_program(GLSLC_PROG glslc)
if (NOT GLSLC_PROG)
    message(WARN "can't find glslc, won't auto compile shaders")
endif()
include(cmake/target_category_helper.cmake)
include(cmake/compile_shader.cmake)

add_subdirectory(engine)

if (NICKEL_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
