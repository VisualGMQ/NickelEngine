AddRHITest(rhi_cube)

if (EMSCRIPTEN)
    target_link_options(rhi_cube
        PRIVATE
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/shaders@test/testbed/rhi/cube/shaders
            --use-preload-plugins)
endif()