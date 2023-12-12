#pragma once

#include "nickel.hpp"
#include "imgui_plugin.hpp"

class AssetPropertyWindowContext {
public:
    nickel::gogl::Sampler sampler;  // saved sampler for TexturePropertyPopupWindow
};

/**
 * @brief [ImGui] Declare a popup window that show texture properties
 * @note Don't forget reset AssetPropertyWindowContext::sampler before open it
 *
 * @param title
 * @return true     texture reimported
 * @return false
 */
bool TexturePropertyPopupWindow(const std::string& title,
                           nickel::TextureHandle, AssetPropertyWindowContext&);