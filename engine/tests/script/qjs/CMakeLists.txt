macro(AddQJSScriptTest name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE qjs)
    mark_as_cli_test(${name} script/qjs)
endmacro()

file(GLOB JS_FILES ./*.js ./*.mjs)
AddQJSScriptTest(qjs_test)
target_sources(qjs_test PRIVATE ${JS_FILES})