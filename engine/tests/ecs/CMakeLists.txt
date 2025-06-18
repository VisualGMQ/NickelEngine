macro(AddECSTest name)
    add_executable(${name} ${name}.cpp)
    mark_as_cli_test(${name} ecs)
endmacro()

AddECSTest(sparse_set)