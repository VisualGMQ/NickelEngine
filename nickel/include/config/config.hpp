#pragma once

#include <string_view>

namespace nickel {

namespace config {

constexpr int GLMajorVersion = 4;
constexpr int GLMinorVersion = 5;

constexpr int MaxDrawCallNum = 512;
constexpr int MaxVertexPerBatch = 1024;

constexpr std::string_view DefaultWindowTitle = "NickelEngine";
constexpr int DefaultWindowWidth = 720;
constexpr int DefaultWindowHeight = 680;

constexpr std::string_view ProjectConfigFilename = "project.toml";

}  // namespace config

}  // namespace nickel