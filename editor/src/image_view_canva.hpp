#pragma once

#include "nickel.hpp"
#include "imgui_plugin.hpp"

/**
 * @brief [ImGui] show a movable&scalable image widget
 */
void ShowImage(const nickel::cgmath::Vec2& canvaSize,
               nickel::cgmath::Vec2& offset, float& scale,
               nickel::TextureHandle handle);