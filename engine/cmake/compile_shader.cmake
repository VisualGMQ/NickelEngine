macro(compile_shader shader_name output_name)
    if (GLSLC_PROG)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${output_name}
            COMMAND ${GLSLC_PROG}  ${CMAKE_CURRENT_SOURCE_DIR}/${shader_name} -o ${CMAKE_CURRENT_SOURCE_DIR}/${output_name}
            COMMENT "compiling shader ${CMAKE_CURRENT_SOURCE_DIR}/${shader_name} -> ${output_name}"
            MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${shader_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
        )
    else()
        message(WARN "don't find glslc, can't compiling shader ${CMAKE_CURRENT_SOURCE_DIR}/${shader_name}")
    endif()
endmacro()