cmake_minimum_required(VERSION 3.21)
project(NickelEngine
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(engine_name nickel)

add_subdirectory(3rdlibs)
add_subdirectory(nickel)

option(NICKEL_BUILD_SANDBOX "build sandbox" ON)
if (NICKEL_BUILD_SANDBOX)
    add_subdirectory(sandbox)
endif()

option(NICKEL_BUILD_EDITOR "build editor" ON)
if (NICKEL_BUILD_EDITOR)
    add_subdirectory(editor)
endif()

option(NICKEL_BUILD_UNITTEST "build unit test" OFF)
if (NICKEL_BUILD_UNITTEST OR PROJECT_IS_TOP_LEVEL)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()
