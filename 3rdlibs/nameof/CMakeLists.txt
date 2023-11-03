add_library(nameof INTERFACE)
target_include_directories(nameof INTERFACE ./)
target_compile_features(nameof INTERFACE cxx_std_17)
