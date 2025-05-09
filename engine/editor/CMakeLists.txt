add_compile_definitions(-DNAPI_VERSION=4)

file(GLOB EDITOR_SRC cpp/*.hpp cpp/*.cpp)
set(EDITOR_NAME "editor")

add_library(${EDITOR_NAME} SHARED ${EDITOR_SRC} ${CMAKE_JS_SRC})
set_target_properties(${EDITOR_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_include_directories(${EDITOR_NAME} PRIVATE ${CMAKE_JS_INC})
target_link_libraries(${EDITOR_NAME} PRIVATE ${CMAKE_JS_LIB})
target_compile_features(${EDITOR_NAME} PRIVATE cxx_std_20)

if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
  execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()