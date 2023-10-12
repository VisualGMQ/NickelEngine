macro(AddBench name)
    add_executable(${name}_bench ${name}.cpp) 
    target_link_libraries(${name}_bench PRIVATE nanobench ${engine_name})
endmacro(AddBench)

AddBench(gjk)
