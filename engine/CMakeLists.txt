cmake_minimum_required(VERSION 3.20)

project(NickelEngine
    VERSION 0.1.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(NICKEL_TESTS "build tests" OFF)

set(NICKEL_MAIN_ENTRY_NAME NickelEngine-Main)
set(NICKEL_ENGINE_NAME NickelEngine)

include(cmake/target_category_helper.cmake)

add_subdirectory(engine)

if (NICKEL_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
