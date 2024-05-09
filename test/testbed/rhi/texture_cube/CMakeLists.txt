AddRHITest(rhi_texture_cube)
target_link_libraries(rhi_texture_cube PRIVATE stb_image)

if (EMSCRIPTEN)
    target_link_options(rhi_texture_cube
        PRIVATE
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/resources@test/testbed/rhi/texture_cube/resources
            --use-preload-plugins)
endif()