aux_source_directory(. SANDBOX_SRC)

if (ANDROID)
    add_library(main SHARED ${SANDBOX_SRC})
    target_link_libraries(main PRIVATE ${NICKEL_ENGINE_NAME})
else()
    add_executable(sandbox ${SANDBOX_SRC})
    target_link_libraries(sandbox PRIVATE ${NICKEL_ENGINE_NAME} ${NICKEL_MAIN_ENTRY_NAME})

    mark_as_gui_test(sandbox)
endif()
