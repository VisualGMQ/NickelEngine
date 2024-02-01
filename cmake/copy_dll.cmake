macro(CopyDLL target_name)
    if (NICKEL_COPYDLL)
        if (sdl_dll)
            add_custom_command(TARGET ${target_name}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${sdl_dll}> $<TARGET_FILE_DIR:${target_name}>
                COMMAND_EXPAND_LISTS)
        endif()
    endif()
endmacro(CopyDLL)