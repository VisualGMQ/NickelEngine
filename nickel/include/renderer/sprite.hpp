#pragma once

#include "pch.hpp"
#include "core/cgmath.hpp"
#include "misc/transform.hpp"
#include "renderer/camera.hpp"
#include "renderer/renderer2d.hpp"
#include "renderer/texture.hpp"
#include "renderer/context.hpp"


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

    static Sprite FromTexture(TextureHandle texture) {
        Sprite sprite;
        sprite.texture = texture;
        return sprite;
    }

    static Sprite FromRegion(TextureHandle texture,
                             const cgmath::Rect& region) {
        Sprite sprite;
        sprite.region = region;
        sprite.customSize = region.size;
        sprite.texture = texture;
        return sprite;
    }

    static Sprite FromCustomSize(TextureHandle texture,
                                 const cgmath::Vec2& size) {
        Sprite sprite;
        sprite.customSize = size;
        sprite.texture = texture;
        return sprite;
    }
};

struct SpriteBundle final {
    Sprite sprite;
    Transform transform;
};

void RenderSprite(gecs::querier<Sprite, Transform>,
                  gecs::resource<gecs::mut<Renderer2D>>,
                  gecs::resource<TextureManager>, gecs::resource<Camera>,
                  gecs::resource<gecs::mut<RenderContext>> renderCtx);

}  // namespace nickel
