AddRHITest(rhi_triangle)

if (EMSCRIPTEN)
    target_link_options(rhi_triangle
        PRIVATE
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/shaders@test/testbed/rhi/triangle/shaders
            --use-preload-plugins)
endif()