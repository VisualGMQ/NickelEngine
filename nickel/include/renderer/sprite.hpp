#pragma once

#include "core/cgmath.hpp"
#include "misc/hierarchy.hpp"
#include "misc/transform.hpp"
#include "renderer/camera.hpp"
#include "renderer/context.hpp"
#include "renderer/renderer2d.hpp"
#include "renderer/texture.hpp"

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

class AssetManager;

void CollectSpriteRenderInfo(
    gecs::querier<Sprite, Transform, gecs::without<Parent>>,
    gecs::querier<Sprite, Transform, GlobalTransform, Child,
                  gecs::without<Parent>>,
    gecs::resource<AssetManager>, gecs::resource<gecs::mut<RenderContext>>,
    gecs::registry);

void RenderElements(gecs::resource<gecs::mut<Renderer2D>> renderer2d,
                    gecs::resource<gecs::mut<RenderContext>> ctx,
                    gecs::resource<Camera> camera);

}  // namespace nickel
