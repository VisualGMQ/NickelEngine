file(GLOB_RECURSE VEHICLE_SRC ./*.hpp ./*.cpp ./assets/*.*)

add_executable(vehicle ${VEHICLE_SRC})
target_link_libraries(vehicle PRIVATE tomlplusplus::tomlplusplus)
mark_as_gui_test(vehicle physics)
