cmake_minimum_required(VERSION 3.25)

project(NickelEngine
    DESCRIPTION "a game engine based on ECS"
    VERSION 0.1.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(NICKEL_BUILD_TESTS "build tests" OFF)
option(NICKEL_BUILD_TOOLS "build tools" OFF)

set(CMAKE_VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(NICKEL_MAIN_ENTRY_NAME NickelEngine-Main)
set(NICKEL_ENGINE_NAME NickelEngine)

find_program(GLSLC_PROG glslc)
if (NOT GLSLC_PROG)
    message(WARN "can't find glslc, won't auto compile shaders")
endif()
include(cmake/target_category_helper.cmake)
include(cmake/compile_shader.cmake)
include(cmake/add_3rdlib.cmake)


## generate path content
file(READ ./nickel_engine_project_path_template.toml PROJECT_PATH_TEMPLATE_CONTENT)
cmake_path(RELATIVE_PATH CMAKE_CURRENT_SOURCE_DIR
            BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE NICKEL_ENGINE_RELATIVE_PATH)
string(REPLACE "#PROJECT_PATH#" "${NICKEL_ENGINE_RELATIVE_PATH}" PROJECT_PATH_CONTENT ${PROJECT_PATH_TEMPLATE_CONTENT})

file(WRITE ${CMAKE_SOURCE_DIR}/nickel_engine_project_path.toml ${PROJECT_PATH_CONTENT})

## build

add_subdirectory(engine)

if (NICKEL_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

add_subdirectory(doc)

if (NICKEL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()