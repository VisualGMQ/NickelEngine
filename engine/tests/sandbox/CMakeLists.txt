aux_source_directory(. SANDBOX_SRC)

if (ANDROID)
    add_library(main SHARED ${SANDBOX_SRC})
    target_link_libraries(main PRIVATE NickelEngine)
else()
    add_executable(sandbox ${SANDBOX_SRC})
    target_link_libraries(sandbox PRIVATE NickelEngine)

    mark_as_test(sandbox)
endif()
