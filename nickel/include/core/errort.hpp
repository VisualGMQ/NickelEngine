#pragma once

#include <string_view>

namespace nickel {

constexpr std::string_view GlfwErr = "GLFW";
constexpr std::string_view GladErr = "Glad";
constexpr std::string_view GLErr = "GL";
constexpr std::string_view ConfigErr = "Config";
constexpr std::string_view ResErr = "Resource";
constexpr std::string_view RendererErr = "Renderer";

}