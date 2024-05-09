AddRHITest(rhi_normal_map)
target_link_libraries(rhi_normal_map PRIVATE stb_image)

if (EMSCRIPTEN)
    target_link_options(rhi_normal_map
        PRIVATE
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/resources@test/testbed/rhi/normal_map/resources
            --use-preload-plugins)
endif()