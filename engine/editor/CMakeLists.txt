add_compile_definitions(-DNAPI_VERSION=4)

file(GLOB EDITOR_SRC cpp/*.hpp cpp/*.cpp)
set(EDITOR_NAME "editor")

add_library(${EDITOR_NAME} SHARED ${EDITOR_SRC} ${CMAKE_JS_SRC})
set_target_properties(${EDITOR_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${EDITOR_NAME} PRIVATE ${CMAKE_JS_LIB} ${NICKEL_ENGINE_NAME})
target_compile_features(${EDITOR_NAME} PRIVATE cxx_std_20)

# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${EDITOR_NAME} PRIVATE ${NODE_ADDON_API_DIR} ${CMAKE_JS_INC})

if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
  execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()

file(MAKE_DIRECTORY addon)

add_custom_command(TARGET ${EDITOR_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${EDITOR_NAME}> addon
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "copying nodejs cpp binding code..."
  VERBATIM
)