
# Nickel.Common
aux_source_directory(src/common nickel_common_src)
file(GLOB_RECURSE nickel_common_headers ./include/common/*.hpp)

add_library(Nickel.Common STATIC)
target_sources(Nickel.Common PRIVATE ${nickel_common_src})
target_sources(Nickel.Common PUBLIC ${nickel_common_headers})
target_include_directories(Nickel.Common PUBLIC ./include)
set_target_properties(Nickel.Common
    PROPERTIES
    PUBLIC_HEADER "${nickel_common_headers}")
target_compile_features(Nickel.Common PUBLIC cxx_std_17)
target_link_libraries(Nickel.Common PUBLIC mirrow gecs)
target_link_libraries(Nickel.Common PRIVATE debugbreak "$<$<CONFIG:Debug>:easy_profiler>")
target_compile_definitions(Nickel.Common PUBLIC "$<$<CONFIG:Debug>:NICKEL_ENABLE_PROFILE>" _CRT_SECURE_NO_WARNINGS)

# Nickel.Graphics
aux_source_directory(src/graphics nickel_graphics_src)
aux_source_directory(src/graphics/rhi nickel_graphics_src)
if (TARGET Vulkan)
    aux_source_directory(src/graphics/rhi/vk nickel_graphics_src)
endif()
aux_source_directory(src/graphics/rhi/gl4 nickel_graphics_src)
aux_source_directory(src/graphics/rhi/impl nickel_graphics_src)
aux_source_directory(src/graphics/rhi/null nickel_graphics_src)
file(GLOB_RECURSE nickel_graphics_headers ./include/graphics/*.hpp)

add_library(Nickel.Graphics STATIC)
target_sources(Nickel.Graphics PRIVATE ${nickel_graphics_src})
target_sources(Nickel.Graphics PUBLIC ${nickel_graphics_headers})
target_include_directories(Nickel.Graphics PUBLIC ./include)
set_target_properties(Nickel.Graphics
    PROPERTIES
    PUBLIC_HEADER "${nickel_graphics_headers}")
target_compile_features(Nickel.Graphics PUBLIC cxx_std_17)
target_link_libraries(Nickel.Graphics PUBLIC glad)
target_link_libraries(Nickel.Graphics PRIVATE stb_image rapidxml freetype lunasvg tinygltf SDL2 Nickel.Common)
target_compile_definitions(Nickel.Graphics PUBLIC
    TINYGLTF_NO_STB_IMAGE 
    TINYGLTF_NO_STB_IMAGE_WRITE
    TINYGLTF_NO_EXTERNAL_IMAGE
    TINYGLTF_NO_INCLUDE_STB_IMAGE
    TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
    TINYGLTF_USE_CPP14
)
if (TARGET Vulkan)
   target_link_libraries(Nickel.Graphics PRIVATE Vulkan)
   target_compile_definitions(Nickel.Graphics PUBLIC NICKEL_HAS_VULKAN)
endif()

# Nickel.Geometry
aux_source_directory(src/geom nickel_geom_src)
file(GLOB_RECURSE nickel_geom_headers ./include/geom/*.hpp)

add_library(Nickel.Geometry STATIC)
target_sources(Nickel.Geometry PRIVATE ${nickel_geom_src})
target_sources(Nickel.Geometry PUBLIC ${nickel_geom_headers})
target_include_directories(Nickel.Geometry PUBLIC ./include)
set_target_properties(Nickel.Geometry
    PROPERTIES
    PUBLIC_HEADER "${nickel_geom_headers}")
target_compile_features(Nickel.Geometry PUBLIC cxx_std_17)
target_link_libraries(Nickel.Geometry PUBLIC Nickel.Common)

# Nickel.Video
aux_source_directory(src/video nickel_video_src)
file(GLOB_RECURSE nickel_video_headers ./include/video/*.hpp)

add_library(Nickel.Video STATIC)
target_sources(Nickel.Video PRIVATE ${nickel_video_src})
target_sources(Nickel.Video PUBLIC ${nickel_video_headers})
target_include_directories(Nickel.Video PUBLIC ./include)
set_target_properties(Nickel.Video
    PROPERTIES
    PUBLIC_HEADER "${nickel_video_headers}")
target_compile_features(Nickel.Video PUBLIC cxx_std_17)
# remove Nickel.Graphics(due to window depedent opengl)
target_link_libraries(Nickel.Video PUBLIC Nickel.Common Nickel.Graphics)
target_link_libraries(Nickel.Video PRIVATE SDL2)

# Nickel.Physics
aux_source_directory(src/physics nickel_physics_src)
file(GLOB_RECURSE nickel_physics_headers ./include/physics/*.hpp)

add_library(Nickel.Physics STATIC)
target_sources(Nickel.Physics PRIVATE ${nickel_physics_src})
target_sources(Nickel.Physics PUBLIC ${nickel_physics_headers})
target_include_directories(Nickel.Physics PUBLIC ./include)
set_target_properties(Nickel.Physics
    PROPERTIES
    PUBLIC_HEADER "${nickel_physics_headers}")
target_compile_features(Nickel.Physics PUBLIC cxx_std_17)
target_link_libraries(Nickel.Physics PUBLIC Nickel.Common)

# Nickel.Audio
aux_source_directory(src/audio nickel_audio_src)
file(GLOB_RECURSE nickel_audio_headers ./include/audio/*.hpp)

add_library(Nickel.Audio STATIC)
target_sources(Nickel.Audio PRIVATE ${nickel_audio_src})
target_sources(Nickel.Audio PUBLIC ${nickel_audio_headers})
target_include_directories(Nickel.Audio PUBLIC ./include)
set_target_properties(Nickel.Audio
    PROPERTIES
    PUBLIC_HEADER "${nickel_audio_headers}")
target_compile_features(Nickel.Audio PUBLIC cxx_std_17)
target_link_libraries(Nickel.Audio PUBLIC Nickel.Common)
target_link_libraries(Nickel.Audio PRIVATE miniaudio)

# Nickel.Animation
aux_source_directory(src/anim nickel_anim_src)
file(GLOB_RECURSE nickel_anim_headers ./include/anim/*.hpp)

add_library(Nickel.Animation STATIC)
target_sources(Nickel.Animation PRIVATE ${nickel_anim_src})
target_sources(Nickel.Animation PUBLIC ${nickel_anim_headers})
target_include_directories(Nickel.Animation PUBLIC ./include)
set_target_properties(Nickel.Animation
    PROPERTIES
    PUBLIC_HEADER "${nickel_anim_headers}")
target_compile_features(Nickel.Animation PUBLIC cxx_std_17)
target_link_libraries(Nickel.Animation PUBLIC Nickel.Common)

# Nickel.UI
aux_source_directory(src/ui nickel_ui_src)
file(GLOB_RECURSE nickel_ui_headers ./include/ui/*.hpp)

add_library(Nickel.UI STATIC)
target_sources(Nickel.UI PRIVATE ${nickel_ui_src})
target_sources(Nickel.UI PUBLIC ${nickel_ui_headers})
target_include_directories(Nickel.UI PUBLIC ./include)
set_target_properties(Nickel.UI
    PROPERTIES
    PUBLIC_HEADER "${nickel_ui_headers}")
target_compile_features(Nickel.UI PUBLIC cxx_std_17)
target_link_libraries(Nickel.UI PUBLIC Nickel.Common Nickel.Graphics)

# Nickel.Refl
aux_source_directory(src/refl nickel_refl_src)
file(GLOB_RECURSE nickel_refl_headers ./include/refl/*.hpp)

add_library(Nickel.Refl STATIC)
target_sources(Nickel.Refl PRIVATE ${nickel_refl_src})
target_sources(Nickel.Refl PUBLIC ${nickel_refl_headers})
target_include_directories(Nickel.Refl PUBLIC ./include)
set_target_properties(Nickel.Refl
    PROPERTIES
    PUBLIC_HEADER "${nickel_refl_headers}")
target_compile_features(Nickel.Refl PUBLIC cxx_std_17)
target_link_libraries(Nickel.Refl PUBLIC
    Nickel.Common
    Nickel.Graphics
    Nickel.Animation
    Nickel.Audio
    Nickel.Physics
    Nickel.UI)

# Nickel.Misc
aux_source_directory(src/misc nickel_misc_src)
aux_source_directory(src/system nickel_misc_src)
file(GLOB_RECURSE nickel_misc_headers ./include/misc/*.hpp ./include/system/*.hpp)

add_library(Nickel.Misc STATIC)
target_sources(Nickel.Misc PRIVATE ${nickel_misc_src})
target_sources(Nickel.Misc PUBLIC ${nickel_misc_headers})
target_include_directories(Nickel.Misc PUBLIC ./include)
set_target_properties(Nickel.Misc
    PROPERTIES
    PUBLIC_HEADER "${nickel_misc_headers}")
target_compile_features(Nickel.Misc PUBLIC cxx_std_17)
target_link_libraries(Nickel.Misc PUBLIC
    Nickel.Common
    Nickel.Graphics
    Nickel.Physics
    Nickel.Animation
    Nickel.Audio
    Nickel.Refl)

# Nickel.Nickel
add_library(Nickel.Nickel INTERFACE)
target_link_libraries(Nickel.Nickel INTERFACE
    Nickel.Common
    Nickel.Graphics
    Nickel.Physics
    Nickel.Animation
    Nickel.Audio
    Nickel.Misc
    Nickel.Video
    Nickel.Refl)

# Nickel.Runtime
aux_source_directory(src/runtime nickel_runtime_src)
add_library(Nickel.Runtime STATIC)
target_sources(Nickel.Runtime PRIVATE ${nickel_runtime_src})
target_link_libraries(Nickel.Runtime PUBLIC Nickel.Nickel)
# TODO: remove SDL2
target_link_libraries(Nickel.Runtime PRIVATE SDL2 toml++)


# install
install(TARGETS Nickel.Common
    EXPORT Nickel.Common
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/lib
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/bin
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/archive
    PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/include/core
    COMPONENT Nickel.Common)

install(TARGETS Nickel.Graphics
    EXPORT Nickel.Graphics
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/lib
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/bin
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/archive
    PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/include/graphics
    COMPONENT Nickel.Graphics)

install(TARGETS Nickel.Physics
    EXPORT Nickel.Physics
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/lib
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/bin
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/archive
    PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/include/physics
    COMPONENT Nickel.Physics)

install(TARGETS Nickel.Animation
    EXPORT Nickel.Animation
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/lib
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/bin
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/archive
    PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/include/anim
    COMPONENT Nickel.Animation)

install(TARGETS Nickel.Audio
    EXPORT Nickel.Audio
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/lib
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/bin
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/archive
    PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/include/audio
    COMPONENT Nickel.Audio)

install(TARGETS Nickel.Misc
    EXPORT Nickel.Misc
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/lib
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/bin
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/archive
    PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/include/misc
    COMPONENT Nickel.Misc)

install(TARGETS Nickel.Video
    EXPORT Nickel.Video
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/lib
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/bin
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/archive
    PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/include/video
    COMPONENT Nickel.Video)

install(TARGETS Nickel.Refl
    EXPORT Nickel.Refl
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/lib
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/bin
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/archive
    PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/include/refl
    COMPONENT Nickel.Refl)

install(TARGETS Nickel.Nickel
    EXPORT Nickel.Nickel
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/lib
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/bin
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/archive
    COMPONENT Nickel.Nickel)

install(TARGETS Nickel.Runtime
    EXPORT Nickel.Runtime
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/lib
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/bin
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/install/nickel/archive
    COMPONENT Nickel.Runtime)