file(GLOB_RECURSE VEHICLE_SRC ./*.hpp ./*.cpp ./assets/*.*)

add_executable(test_vehicle ${VEHICLE_SRC})
target_link_libraries(test_vehicle PRIVATE tomlplusplus::tomlplusplus)
mark_as_gui_test(test_vehicle)
