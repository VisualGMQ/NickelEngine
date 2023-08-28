cmake_minimum_required(VERSION 3.12)
project(NickelEngine
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(engine_name nickel)

add_subdirectory(3rdlibs)
add_subdirectory(nickel)

option(NICKEL_BUILD_EDITOR "build editor" ON)
if (NICKEL_BUILD_EDITOR)
    add_subdirectory(editor)
endif()
