find_package(Python3 COMPONENTS Interpreter)

file(GLOB_RECURSE PARSER_FILES ./test/*.hpp ./test/*.cpp ./mustache/*.mustache)

add_custom_target(
    code_generate
    COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/parser.py ../nickel ${NICKEL_GENERATE_DIR} ${NICKEL_REFL_GENERATE_DIR} ${NICKEL_BINDING_GENERATE_DIR}
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "running code generator"
    SOURCES parser.py requirements.txt ${PARSER_FILES})

set_target_properties(code_generate
    PROPERTIES
    FOLDER tools)