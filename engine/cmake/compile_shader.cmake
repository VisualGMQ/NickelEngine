macro(compile_shader shader_files)
    foreach(file ${shader_files})
        set(SHADER_COMPILED_FILE ${file}.spv)
        add_custom_command(
            OUTPUT ${SHADER_COMPILED_FILE}
            COMMAND ${GLSLC_PROG} ${file} -o ${SHADER_COMPILED_FILE}
            COMMENT "compiling shader ${file} -> ${SHADER_COMPILED_FILE}"
            MAIN_DEPENDENCY ${file}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
        )
    endforeach()
endmacro()