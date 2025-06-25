configure_file(./Doxyfile.inc ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

find_package(Doxygen QUIET)

if (NOT DOXYGEN_FOUND)
    message(WARNING "can't find doxygen, can't generate document")
else()
    add_custom_target(doc
        COMMAND Doxygen::doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        COMMENT "generate document ..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM
        SOURCES Doxyfile.inc)
endif()