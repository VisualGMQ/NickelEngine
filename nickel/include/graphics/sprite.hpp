#pragma once

#include "common/cgmath.hpp"
#include "common/hierarchy.hpp"
#include "common/transform.hpp"
#include "graphics/camera.hpp"
#include "graphics/context.hpp"
#include "graphics/renderer2d.hpp"
#include "graphics/texture.hpp"

namespace nickel {

/**
 * @brief a component that present a sprite
 */
struct Sprite final {
    cgmath::Color color = {1.0, 1.0, 1.0, 1.0};
    std::optional<cgmath::Rect> region;
    std::optional<cgmath::Vec2> customSize;
    cgmath::Vec2 anchor;
    TextureHandle texture;
    Flip flip = Flip::None;
    bool visiable = true;
    int zIndex = 0;

    static Sprite FromTexture(TextureHandle texture);
    static Sprite FromRegion(TextureHandle texture,
                             const cgmath::Rect& region);
    static Sprite FromCustomSize(TextureHandle texture,
                                 const cgmath::Vec2& size);
};

struct SpriteBundle final {
    Sprite sprite;
    Transform transform;
};

}  // namespace nickel
