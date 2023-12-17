macro(CopyDLL target_name)
    if (NICKEL_COPYDLL)
        add_custom_command(TARGET ${target_name}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${target_name}> $<TARGET_FILE_DIR:${target_name}>
            COMMAND_EXPAND_LISTS)
    endif()
endmacro(CopyDLL)