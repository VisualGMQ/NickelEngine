#pragma once

#include "common/cgmath.hpp"
#include "common/hierarchy.hpp"
#include "common/transform.hpp"
#include "graphics/camera.hpp"
#include "graphics/context.hpp"
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
    Flip flip = Flip::None;
    bool visiable = true;
    int orderInLayer = 0;

    static Sprite FromRegion(const cgmath::Rect& region);
    static Sprite FromCustomSize(const cgmath::Vec2& size);
};

struct SpriteMaterial {
    Material2DHandle material;
};

struct SpriteBundle final {
    Transform transform;
    Sprite sprite;
    SpriteMaterial material;
};

}  // namespace nickel
