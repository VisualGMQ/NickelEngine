# download SDL for github action
message("find package: SDL2 ...")
find_package(SDL2 QUIET)

# check SDL
if (SDL2_FOUND)
    if (NOT EXISTS "${SDL2_INCLUDE_DIRS}/SDL.h")
        set(SDL2_FOUND OFF)
    else()
        add_library(SDL2 INTERFACE IMPORTED GLOBAL)
        target_link_libraries(SDL2 INTERFACE SDL2::SDL2 SDL2::SDL2main)
        message("SDL2 information:")
        message("\tinclude dir: ${SDL2_INCLUDE_DIRS}")
        message("\tSDL libs: ${SDL2_LIBRARIES}")
    endif()
endif()

if (NOT SDL2_FOUND)
    message("cmake find SDL2 failed! use pkg-config...")

    find_package(PkgConfig QUIET)
    if (PKG_CONFIG_FOUND)
        message("pkg-config found OK")
        pkg_check_modules(SDL2 sdl2 QUIET IMPORTED_TARGET)
        if (SDL2_FOUND)
            add_library(SDL2 ALIAS PkgConfig::SDL2)
        else() 
            message("pkg-config can't find SDL2!")
        endif()
    else()
        message("found pkg-config failed!")
    endif()
endif()

# config SDL2 for windows
if (WIN32 AND NOT SDL2_FOUND)
    set(SDL2_ROOT "" CACHE PATH "SDL2 root directory")

    if (NOT SDL2_ROOT)
        message(FATAL_ERROR "found SDL2 failed! please set SDL2_ROOT to SDL2 lib root path")
    endif()

    # 1 - GNU-like
    # 2 - MSVC
    set(compiler_type 1)    

    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
        set(compiler_type 2)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
        set(compiler_type 2)
    endif()

    # 1 - x86
    # 2 - x64
    set(arch_type 1)

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(arch_type 2)
    endif()

    # some var to store SDL compile info
    set(sdl_include_dir "")
    set(sdlmain_lib "")
    set(sdl_lib "")
    set(sdl_dll "")
    set(sdl_otherlibs "")

    if (${compiler_type} EQUAL 1) # for GNU like compiler
        message("your compiler is GNU-like")

        set(sdl_dir ${SDL2_ROOT}/i686-w64-mingw32)
        if (${arch_type} EQUAL 2)
            set(sdl_dir ${SDL2_ROOT}/x86_64-w64-mingw32)
        endif()

        set(sdl_include_dir ${sdl_dir}/include/SDL2)
        set(sdl_static_lib_dir ${sdl_dir}/lib)
        set(sdlmain_lib ${sdl_static_lib_dir}/libSDL2main.a)
        set(sdl_lib ${sdl_static_lib_dir}/libSDL2main.a)
        set(sdl_dll ${sdl_dir}/bin/SDL2.dll)
        set(sdl_otherlibs "-lmingw32 -lSDL2main -lSDL2")
    else() # for MSVC compiler
        message("your compiler is based on MSVC")
        set(sdl_lib_dir ${SDL2_ROOT}/lib/x86)

        if (${arch_type} EQUAL 2)
            set(sdl_lib_dir ${SDL2_ROOT}/lib/x64)
        endif()

        set(sdl_lib ${sdl_lib_dir}/SDL2.lib)
        set(sdlmain_lib ${sdl_lib_dir}/SDL2main.lib)
        set(sdl_include_dir ${SDL2_ROOT}/include)
        set(sdl_dll ${sdl_lib_dir}/SDL2.dll)
    endif()

    # SDL2 main
    add_library(SDL2main STATIC IMPORTED)
    set_target_properties(SDL2main
        PROPERTIES
            IMPORTED_LOCATION ${sdlmain_lib}
            IMPORTED_LINK_INTERFACE_LANGUAGES C)

    add_library(SDL2core SHARED IMPORTED)
    target_include_directories(SDL2core INTERFACE ${sdl_include_dir})
    set_target_properties(SDL2core
        PROPERTIES
            IMPORTED_IMPLIB ${sdl_lib}
            IMPORTED_LOCATION ${sdl_dll}
            IMPORTED_LINK_INTERFACE_LANGUAGES C)

    add_library(SDL2 INTERFACE IMPORTED GLOBAL)
    target_link_libraries(SDL2 INTERFACE SDL2main SDL2core)

    message("SDL2 information:")
    message("\tinclude dir: ${SDL2_ROOT}/include")
    message("\tSDL main library dir: ${sdlmain_lib}")
    message("\tSDL lib library dir: ${sdl_lib}")
    message("\tdynamic lib: ${sdl_dll}")

    set(SDL2_FOUND ON)
endif()

if (NOT WIN32 AND NOT SDL2_FOUND)
    message(FATAL_ERROR "can't find SDL2!")
endif()