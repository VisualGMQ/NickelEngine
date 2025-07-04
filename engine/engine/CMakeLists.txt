add_subdirectory(3rdlibs)

option(NICKEL_BUILD_SHARED "build shared engine library" OFF)
option(NICKEL_BUILD_STATIC "build static engine library" ON)

set(NICKEL_GENERATE_INCLUDE_DIR ${CMAKE_BINARY_DIR}/generate/)
set(NICKEL_GENERATE_DIR ${NICKEL_GENERATE_INCLUDE_DIR}/nickel/generate/)
set(NICKEL_REFL_GENERATE_DIR ${NICKEL_GENERATE_DIR}/refl/)
set(NICKEL_BINDING_GENERATE_DIR ${NICKEL_GENERATE_DIR}/binding/)
file(MAKE_DIRECTORY ${NICKEL_REFL_GENERATE_DIR})
file(MAKE_DIRECTORY ${NICKEL_BINDING_GENERATE_DIR})

# copy tirvial file
file(COPY_FILE code_generator/mustache/refl/trivial_header.hpp ${NICKEL_REFL_GENERATE_DIR}/refl_generate.hpp ONLY_IF_DIFFERENT)
file(COPY_FILE code_generator/mustache/refl/trivial_impl.cpp ${NICKEL_REFL_GENERATE_DIR}/refl_generate.cpp ONLY_IF_DIFFERENT)
file(COPY_FILE code_generator/mustache/script/trivial_header.hpp ${NICKEL_BINDING_GENERATE_DIR}/script_binding.hpp ONLY_IF_DIFFERENT)
file(COPY_FILE code_generator/mustache/script/trivial_impl.cpp ${NICKEL_BINDING_GENERATE_DIR}/script_binding.cpp ONLY_IF_DIFFERENT)

add_subdirectory(code_generator)

file(GLOB_RECURSE ENGINE_SRC src/*.cpp)
file(GLOB_RECURSE ENGINE_ALL_HEADERS nickel/*.hpp)
file(GLOB_RECURSE ENGINE_INTERNAL_HEADERS
        nickel/internal/*.hpp
        nickel/graphics/lowlevel/internal/*.hpp
        nickel/graphics/internal/*.hpp
        nickel/physics/internal/*.hpp
        nickel/video/internal/*.hpp
        nickel/refl/internal/*.hpp
        nickel/main_entry/*.hpp)
set(ENGINE_EXPORT_HEADERS ${ENGINE_ALL_HEADERS})
list(REMOVE_ITEM ENGINE_EXPORT_HEADERS ${ENGINE_INTERNAL_HEADERS})
file(GLOB_RECURSE ENGINE_SHADER_FILES assets/shaders/*.frag assets/shaders/*.vert)
file(GLOB_RECURSE ENGINE_MODEL_FILES assets/models/*)
set(ENGINE_GENERATE_FILES ${NICKEL_REFL_GENERATE_DIR}/refl_generate.cpp ${NICKEL_BINDING_GENERATE_DIR}/script_binding.cpp)

set(ENGINE_SHADER_COMPILED_FILES ${ENGINE_SHADER_FILES})
list(TRANSFORM ENGINE_SHADER_COMPILED_FILES APPEND ".spv")
compile_shader("${ENGINE_SHADER_FILES}")

macro(attach_common_config_on_engine target_name)
    target_sources(${target_name} PRIVATE ${ENGINE_ALL_HEADERS} ${ENGINE_SRC} ${ENGINE_MODEL_FILES} ${ENGINE_SHADER_FILES} ${ENGINE_GENERATE_FILES})
    if (GLSLC_PROG)
        target_sources(${target_name} PRIVATE ${ENGINE_SHADER_COMPILED_FILES})
    endif()
    target_include_directories(${target_name} PUBLIC .)
    target_include_directories(${target_name} PRIVATE ${NICKEL_GENERATE_INCLUDE_DIR})
    target_compile_features(${target_name} PUBLIC cxx_std_20)
    target_link_libraries(${target_name} PRIVATE SDL3::SDL3 volk::volk stb_image tinygltf tomlplusplus::tomlplusplus qjs
        # glslang & spriv
        glslang::glslang
        glslang::glslang-default-resource-limits

        # PhysX
        PhysXExtensions
        PhysX
        PhysXPvdSDK
        PhysXVehicle
        PhysXCharacterKinematic
        PhysXCooking
        PhysXCommon
        PhysXFoundation
        
        # NvBlast
        NvBlast::LowLevel
        NvBlast::Globals
        NvBlast::ExtShaders
        NvBlast::ExtAssetUtils
        NvBlast::ExtAuthoring
        NvBlast::Tk
        NvBlast::ExtStress
    )
    target_link_libraries(${target_name} PUBLIC spdlog::spdlog imgui implot)
    add_dependencies(${target_name} code_generate)

    target_precompile_headers(${target_name} PRIVATE nickel/internal/pch.hpp nickel/physics/internal/pch.hpp)
    target_compile_definitions(${target_name} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(${target_name} PUBLIC $<IF:$<CONFIG:Debug>,NICKEL_DEBUG,NICKEL_RELEASE>)

    if (MSVC)
        target_compile_options(${target_name} PUBLIC /Zc:preprocessor)
    endif()

    if (ANDROID)
        target_compile_definitions(${target_name} PUBLIC NICKEL_PLATFORM_ANDROID)
    elseif(WIN32)
        target_compile_definitions(${target_name} PUBLIC NICKEL_PLATFORM_WIN32)
    elseif(LINUX)
        target_compile_definitions(${target_name} PUBLIC NICKEL_PLATFORM_LINUX)
    elseif(APPLE)
        if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            target_compile_definitions(${target_name} PUBLIC NICKEL_PLATFORM_MACOS)
        else()
            target_compile_definitions(${target_name} PUBLIC NICKEL_PLATFORM_IOS)
        endif()
    endif()
endmacro()

if (NICKEL_BUILD_SHARED)
    set(ENGINE_SHARED_NAME ${NICKEL_ENGINE_NAME}-shared)
    add_library(${ENGINE_SHARED_NAME} SHARED)
    target_compile_definitions(${ENGINE_SHARED_NAME} PUBLIC NICKEL_DEF_DLLEXPORT)
    attach_common_config_on_engine(${ENGINE_SHARED_NAME})
    add_library(${NICKEL_ENGINE_NAME} ALIAS ${ENGINE_SHARED_NAME})
    message("build shared nickel engine")
elseif(NICKEL_BUILD_STATIC)
    set(ENGINE_STATIC_NAME ${NICKEL_ENGINE_NAME}-static)
    add_library(${ENGINE_STATIC_NAME} STATIC)
    attach_common_config_on_engine(${ENGINE_STATIC_NAME})
    add_library(${NICKEL_ENGINE_NAME} ALIAS ${ENGINE_STATIC_NAME})
    message("build static nickel engine")
else()
    message(FATAL_ERROR "must config engine build type(NICKEL_BUILD_SHARED/NICKEL_BUILD_STATIC)")
endif()

add_subdirectory(main_entry)