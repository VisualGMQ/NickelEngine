file(GLOB_RECURSE FILES ./*.hpp ./*.cpp)

add_executable(shader_compiler)
target_sources(shader_compiler PRIVATE ${FILES})
mark_as_tool_without_engine(shader_compiler)
target_link_libraries(shader_compiler PRIVATE 
    ${NICKEL_ENGINE_NAME}
    glslang::glslang
    glslang::glslang-default-resource-limits
    lyra
)