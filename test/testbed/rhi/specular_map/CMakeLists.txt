AddRHITest(rhi_specular_map)
target_link_libraries(rhi_specular_map PRIVATE stb_image)

if (EMSCRIPTEN)
    target_link_options(rhi_specular_map
        PRIVATE
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/resources@test/testbed/rhi/specular_map/resources
            --use-preload-plugins)
endif()