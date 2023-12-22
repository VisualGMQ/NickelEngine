#pragma once

#include <string_view>

namespace nickel {

namespace log_tag {

constexpr std::string_view SDL2 = "SDL2";
constexpr std::string_view Glad = "Glad";
constexpr std::string_view GL = "GL";
constexpr std::string_view Config = "Config";
constexpr std::string_view Asset = "Asset";
constexpr std::string_view Renderer = "Renderer";
constexpr std::string_view Nickel = "Nickel";
constexpr std::string_view Misc = "Misc";
constexpr std::string_view Editor = "Editor";
constexpr std::string_view Audio = "Audio";

}

}