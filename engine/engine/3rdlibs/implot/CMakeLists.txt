aux_source_directory(. IMPLOT_SRC)
add_library(implot STATIC ${IMPLOT_SRC})
target_link_libraries(implot PUBLIC imgui)
target_include_directories(implot PUBLIC .)