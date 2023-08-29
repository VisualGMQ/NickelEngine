add_library(toml++ STATIC ./toml++/toml.cpp)
target_include_directories(toml++ PUBLIC ./)
target_compile_features(toml++ PRIVATE cxx_std_17)
target_compile_definitions(toml++ PUBLIC TOML_HEADER_ONLY=0 TOML_EXCEPTIONS=0)
