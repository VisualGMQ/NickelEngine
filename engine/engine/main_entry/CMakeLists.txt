aux_source_directory(. MAIN_ENTRY_SRC)
file(GLOB_RECURSE ENTRY_HEADERS ./*.hpp)

add_library(${NICKEL_MAIN_ENTRY_NAME} STATIC)
target_sources(${NICKEL_MAIN_ENTRY_NAME} PRIVATE ${MAIN_ENTRY_SRC} ${ENTRY_HEADERS})
target_link_libraries(${NICKEL_MAIN_ENTRY_NAME} PUBLIC ${NICKEL_ENGINE_NAME})
target_link_libraries(${NICKEL_MAIN_ENTRY_NAME} PRIVATE SDL3::SDL3)
target_include_directories(${NICKEL_MAIN_ENTRY_NAME} PUBLIC ../)
