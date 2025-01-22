macro(compile_shader shader_name output_name)
    if (GLSLC_PROG)
        execute_process(
            COMMAND ${GLSLC_PROG}  ${CMAKE_CURRENT_SOURCE_DIR}/${shader_name} -o ${CMAKE_CURRENT_SOURCE_DIR}/${output_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    else()
        message(STATUS "compiling shader ${CMAKE_CURRENT_SOURCE_DIR}: ${shader_name} --> ${output_name}")
    endif()
endmacro()