#pragma once

#include "misc/transform.hpp"
#include "core/cgmath.hpp"
#include "renderer/texture.hpp"
#include "renderer/renderer2d.hpp"
#include "renderer/camera.hpp"
#include "pch.hpp"

namespace nickel {

/**
 * @brief a component that present a sprite
 */
struct Sprite final {
    cgmath::Color color = {1.0, 1.0, 1.0, 1.0};
    std::optional<cgmath::Rect> region;
    std::optional<cgmath::Vec2> customSize;
    cgmath::Vec2 anchor;

    static Sprite Default() {
        return Sprite{
            cgmath::Color{1.0, 1.0, 1.0, 1.0},
            std::nullopt,
            std::nullopt,
            cgmath::Vec2{},
        };
    }

    static Sprite FromRegion(const cgmath::Rect& region) {
        auto sprite = Sprite::Default();
        sprite.region = region;
        sprite.customSize = cgmath::Vec2{region.w, region.h};
        return sprite;
    }

    static Sprite FromCustomSize(const cgmath::Vec2& size) {
        auto sprite = Sprite::Default();
        sprite.customSize = size;
        return sprite;
    }
};

struct SpriteBundle final {
    Sprite sprite;
    TextureHandle image;
    Flip flip = Flip::None;
    bool visiable = true;

    static void RenderSprite(gecs::querier<SpriteBundle, Transform>,
                             gecs::resource<gecs::mut<Renderer2D>>,
                             gecs::resource<TextureManager> textureMgr,
                             gecs::resource<Camera> camera);
};

struct SpriteBundleSolitary final {
    Sprite sprite;
    Texture& image;
    Flip flip = Flip::None;
    bool visiable = true;
};

}  // namespace nickel