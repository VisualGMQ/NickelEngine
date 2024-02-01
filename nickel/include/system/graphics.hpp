#pragma once

#include "graphics/context.hpp"
#include "graphics/sprite.hpp"
#include "common/transform.hpp"
#include "graphics/camera.hpp"

namespace nickel {


class AssetManager;

void CollectSpriteRenderInfo(
    gecs::querier<Sprite, Transform, gecs::without<Parent>>,
    gecs::querier<Sprite, Transform, GlobalTransform, Child,
                  gecs::without<Parent>>,
    gecs::resource<TextureManager>, gecs::resource<gecs::mut<RenderContext>>,
    gecs::registry);

void RenderElements(gecs::resource<gecs::mut<Renderer2D>> renderer2d,
                    gecs::resource<gecs::mut<RenderContext>> ctx,
                    gecs::resource<gecs::mut<Camera>> camera);

void BeginRenderPipeline(gecs::resource<gecs::mut<Renderer2D>>,
                         gecs::resource<gecs::mut<Camera>>,
                         gecs::resource<gecs::mut<RenderContext>>);

void EndRenderPipeline(gecs::resource<gecs::mut<Renderer2D>>);

}