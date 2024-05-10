add_library(imgui_backend_android STATIC imgui_impl_android.cpp)
target_include_directories(imgui_backend_android PUBLIC .)
target_include_directories(imgui_backend_android PRIVATE ../../)