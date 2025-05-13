find_package(Python3 COMPONENTS Interpreter)

file(GLOB_RECURSE PARSER_TEST ./test/*.hpp ./test/*.cpp)

add_custom_target(
    refl_code_generate
    COMMAND Python3::Interpreter parser.py
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCES parser.py requirements.txt ${PARSER_TEST})

set_target_properties(refl_code_generate
    PROPERTIES
    FOLDER tools)