#pragma once

#include "nickel.hpp"
#include "imgui_plugin.hpp"
#include "widget.hpp"

/**
 * @brief [ImGui] show a movable&scalable image widget
 */
class ImageViewCanva final: public Widget {
public:
    ImageViewCanva() = default;

    void Resize(const nickel::cgmath::Vec2& size) { size_ = size; }
    void Update() override;
    void ChangeTexture(nickel::TextureHandle handle) { handle_ = handle; }

private:
    static constexpr float scaleIncStep = 0.1;
    static constexpr float minScaleFactor = 0.0001;

    nickel::cgmath::Vec2 size_ = {200, 200};
    nickel::cgmath::Vec2 offset_;
    float scale_ = 1.0;
    nickel::TextureHandle handle_;
};