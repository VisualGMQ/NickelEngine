#pragma once

#include "imgui_plugin.hpp"
#include "nickel.hpp"

class AssetPropertyWindowContext {
public:
    nickel::gogl::Sampler
        sampler;  // saved sampler for TexturePropertyPopupWindow
};

/**
 * @brief [ImGui] Declare a popup window that show texture properties
 * @note Don't forget reset AssetPropertyWindowContext::sampler before open it
 */
bool TexturePropertyPopupWindow(const std::string& title, nickel::TextureHandle,
                                AssetPropertyWindowContext&);

/**
 * @brief [ImGui] Declare a popup window that show audio properties
 */
bool SoundPropertyPopupWindow(const std::string& title,
                              nickel::AudioHandle handle);
