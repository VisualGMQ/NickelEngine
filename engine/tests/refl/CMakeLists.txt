macro(AddReflTest name)
    add_executable(${name} ${name}.cpp)
    mark_as_cli_test(${name} refl)
endmacro()

add_subdirectory(util)
add_subdirectory(drefl)
add_subdirectory(serd)