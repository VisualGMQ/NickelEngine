add_subdirectory(3rdlibs)


set(ENGINE_NAME NickelEngine)
option(NICKEL_BUILD_SHARED "build shared engine library" OFF)
option(NICKEL_BUILD_STATIC "build static engine library" ON)

file(GLOB_RECURSE ENGINE_SRC src/*.cpp)
file(GLOB_RECURSE ENGINE_ALL_HEADERS nickel/*.hpp)
file(GLOB_RECURSE ENGINE_INTERNAL_HEADERS
        nickel/internal/*.hpp
        nickel/graphics/internal/*.hpp
        nickel/video/internal/*.hpp)
set(ENGINE_EXPORT_HEADERS ${ENGINE_ALL_HEADERS})
list(REMOVE_ITEM ENGINE_EXPORT_HEADERS ${ENGINE_INTERNAL_HEADERS})

macro(attach_common_config_on_engine target_name)
    target_sources(${target_name} PRIVATE ${ENGINE_ALL_HEADERS} ${ENGINE_SRC})
    target_include_directories(${target_name} PUBLIC .)
    target_compile_features(${target_name} PUBLIC cxx_std_20)
    target_link_libraries(${target_name} PRIVATE SDL3::SDL3 spdlog::spdlog volk::volk)
    target_precompile_headers(${target_name} PRIVATE nickel/internal/pch.hpp)
    target_compile_definitions(${target_name} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(${target_name} PRIVATE $<IF:$<CONFIG:Debug>,NICKEL_DEBUG,NICKEL_RELEASE>)
    if (ANDROID)
        target_compile_definitions(${target_name} PRIVATE NICKEL_PLATFORM_ANDROID)
    endif()
    target_compile_definitions(${target_name} PUBLIC SDL_MAIN_USE_CALLBACKS)
endmacro()

if (NICKEL_BUILD_SHARED)
    set(ENGINE_SHARED_NAME ${ENGINE_NAME}-shared)
    add_library(${ENGINE_SHARED_NAME} SHARED)
    attach_common_config_on_engine(${ENGINE_SHARED_NAME})
elseif(NICKEL_BUILD_STATIC)
    set(ENGINE_STATIC_NAME ${ENGINE_NAME}-static)
    add_library(${ENGINE_STATIC_NAME} STATIC)
    attach_common_config_on_engine(${ENGINE_STATIC_NAME})
else()
    message(FATAL_ERROR "must config engine build type(NICKEL_BUILD_SHARED/NICKEL_BUILD_STATIC)")
endif()

if(NICKEL_BUILD_SHARED)
    add_library(${ENGINE_NAME} ALIAS ${ENGINE_SHARED_NAME})
else()
    add_library(${ENGINE_NAME} ALIAS ${ENGINE_STATIC_NAME})
endif()