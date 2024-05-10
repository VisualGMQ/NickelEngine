AddRHITest(rhi_gltf)
target_link_libraries(rhi_gltf PRIVATE stb_image tinygltf)

if (EMSCRIPTEN)
    target_link_options(rhi_gltf
        PRIVATE
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/resources@test/testbed/rhi/gltf/resources
            --use-preload-plugins)
endif()