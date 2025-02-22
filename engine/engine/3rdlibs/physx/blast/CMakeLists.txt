cmake_minimum_required(VERSION 3.25)

project(NvBlast
    VERSION 5.0.6)

add_subdirectory(3rdlibs)

function(blast_sdklib_standrad_setup target_name dir_name)
    file(GLOB_RECURSE SRC source/sdk/${dir_name}/*.cpp source/sdk/common/*.cpp) 
    target_include_directories(${target_name}
        PUBLIC
            include
            source/sdk/common
            include/${dir_name}
            source/sdk/${dir_name}
    )
    target_sources(${target_name} PRIVATE ${SRC})
    if (LINUX)
        target_compile_options(${target_name} PRIVATE -fPIC -pthread -fvisibility=hidden -fnon-call-exceptions -D_FILE_OFFSET_BITS=64 -fabi-version=8)
        target_link_options(${target_name}
            PRIVATE
                -pthread
                --no-undefined
                --disable-new-dtags
                --export-dynamic
        )
        target_link_libraries(${target_name} PRIVATE rt)
    elseif (WIN32)
        target_compile_options(${target_name} PRIVATE /utf-8 /bigobj /permissive-)
        target_link_libraries(${target_name} PUBLIC rpcrt4)
        target_compile_definitions(${target_name} PRIVATE _CRT_NONSTDC_NO_DEPRECATE BOOST_USE_WINDOWS_H=1)
    endif()
    target_compile_features(${target_name} PRIVATE cxx_std_14)
endfunction()

add_library(NvBlast_LowLevel STATIC)
blast_sdklib_standrad_setup(NvBlast_LowLevel lowlevel)
target_include_directories(NvBlast_LowLevel PUBLIC include/shared/NvFoundation)
set_target_properties(NvBlast_LowLevel PROPERTIES FOLDER 3rdlibs/NvBlast)
add_library(NvBlast::LowLevel ALIAS NvBlast_LowLevel)

add_library(NvBlast_Globals STATIC)
blast_sdklib_standrad_setup(NvBlast_Globals globals)
target_include_directories(NvBlast_Globals PUBLIC 
    include/lowlevel
    source/shared/NsFoundation/include
    include/shared/NvFoundation
)
set_target_properties(NvBlast_Globals PROPERTIES FOLDER 3rdlibs/NvBlast)
add_library(NvBlast::Globals ALIAS NvBlast_Globals)

add_library(NvBlast_ExtShaders STATIC)
blast_sdklib_standrad_setup(NvBlast_ExtShaders extensions/shaders)
target_link_libraries(NvBlast_ExtShaders PRIVATE NvBlast_LowLevel NvBlast_Globals)
target_include_directories(NvBlast_ExtShaders PUBLIC 
    include/lowlevel
    include/globals
    include/shared/NvFoundation
    source/shared/NsFoundation/include
)
set_target_properties(NvBlast_ExtShaders PROPERTIES FOLDER 3rdlibs/NvBlast)
add_library(NvBlast::ExtShaders ALIAS NvBlast_ExtShaders)

add_library(NvBlast_ExtAssetUtils STATIC)
blast_sdklib_standrad_setup(NvBlast_ExtAssetUtils extensions/assetutils)
target_include_directories(NvBlast_ExtAssetUtils PUBLIC 
    include/lowlevel
    include/globals
    include/shared/NvFoundation
)
set_target_properties(NvBlast_ExtAssetUtils PROPERTIES FOLDER 3rdlibs/NvBlast)
add_library(NvBlast::ExtAssetUtils ALIAS NvBlast_ExtAssetUtils)

add_library(NvBlast_ExtAuthoring STATIC)
blast_sdklib_standrad_setup(NvBlast_ExtAuthoring extensions/authoring)
target_link_libraries(NvBlast_ExtAuthoring PRIVATE NvBlast_LowLevel NvBlast_Globals Boost::multiprecision)
file(GLOB_RECURSE NVBLAST_EXT_AURHORING 
    source/sdk/extensions/authoringCommon/*.cpp
    source/sdk/extensions/authoring/VHACD/src/*.cpp
)
target_sources(NvBlast_ExtAuthoring PRIVATE ${NVBLAST_EXT_AURHORING})
target_include_directories(NvBlast_ExtAuthoring PUBLIC 
    include/lowlevel
    include/globals
    include/extensions/assetutils
    include/extensions/authoringCommon
    # include/extensions/authoring
    source/sdk/extensions/authoring
    source/sdk/extensions/authoringCommon
    source/sdk/extensions/authoring/VHACD/inc
    source/sdk/extensions/authoring/VHACD/public
    include/shared/NvFoundation
    source/shared/NsFoundation/include
)
set_target_properties(NvBlast_ExtAuthoring PROPERTIES FOLDER 3rdlibs/NvBlast)
add_library(NvBlast::ExtAuthoring ALIAS NvBlast_ExtAuthoring)

add_library(NvBlast_Tk STATIC)
blast_sdklib_standrad_setup(NvBlast_Tk toolkit)
target_link_libraries(NvBlast_Tk PRIVATE NvBlast_LowLevel NvBlast_Globals)
target_include_directories(NvBlast_Tk PUBLIC 
    include/lowlevel
    include/globals
    source/sdk/globals
    include/shared/NvFoundation
    source/shared/NsFoundation/include
    source/shared/NsFileBuffer/include
    source/shared/NvTask/include
)
set_target_properties(NvBlast_Tk PROPERTIES FOLDER 3rdlibs/NvBlast)
add_library(NvBlast::Tk ALIAS NvBlast_Tk)

add_library(NvBlast_ExtStress STATIC)
blast_sdklib_standrad_setup(NvBlast_ExtStress extensions/stress)
target_sources(NvBlast_ExtStress PRIVATE source/shared/stress_solver/stress.cpp)
target_link_libraries(NvBlast_ExtStress PRIVATE NvBlast_LowLevel NvBlast_Globals)
target_include_directories(NvBlast_ExtStress PUBLIC 
    include/lowlevel
    include/globals
    include/shared/NvFoundation
    source/shared/NsFoundation/include
    source/shared/stress_solver
)
if (LINUX)
    target_compile_options(NvBlast_ExtStress PRIVATE "-march=haswell")
endif()
set_target_properties(NvBlast_ExtStress PROPERTIES FOLDER 3rdlibs/NvBlast)
add_library(NvBlast::ExtStress ALIAS NvBlast_ExtStress)