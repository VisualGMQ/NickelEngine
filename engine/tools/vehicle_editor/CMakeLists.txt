file(GLOB_RECURSE FILES ./*.hpp ./*.cpp)

add_executable(vehicle_editor)
target_sources(vehicle_editor PRIVATE ${FILES})
mark_as_tool(vehicle_editor)