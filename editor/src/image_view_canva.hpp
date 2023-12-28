#pragma once

#include "imgui_plugin.hpp"
#include "nickel.hpp"
#include "util.hpp"
#include "widget.hpp"


/**
 * @brief [ImGui] show a movable&scalable image widget
 */
class ImageViewCanva: public Widget {
public:
    ImageViewCanva() = default;

    void Resize(const nickel::cgmath::Vec2& size) { size_ = size; }
    auto& GetSize() const { return size_; }
    float GetContentScale() const { return scale_; }
    auto& GetContentOffset() const { return offset_; }

    void Update() override;

    void ChangeTexture(nickel::TextureHandle handle) { handle_ = handle; }

protected:
    virtual void additionalDraw(ImDrawList*, const nickel::Texture&,
                                const nickel::cgmath::Vec2& canvasMin) {}

    nickel::cgmath::Vec2 transformPt(const nickel::cgmath::Vec2& p,
                                     const nickel::cgmath::Vec2& offset = {}) {
        return ScaleByAnchor(p, scale_, size_ * 0.5,
                             size_ * 0.5 + offset_ + offset);
    }

private:
    static constexpr float scaleIncStep = 0.1;
    static constexpr float minScaleFactor = 0.0001;

    nickel::cgmath::Vec2 size_ = {200, 200};
    nickel::cgmath::Vec2 offset_;
    float scale_ = 1.0;
    nickel::TextureHandle handle_;
};