#pragma once

#include "common/transform.hpp"
#include "graphics/camera.hpp"
#include "graphics/context.hpp"
#include "graphics/gltf.hpp"
#include "graphics/sprite.hpp"
#include "video/window.hpp"


namespace nickel {

void UpdateCamera2GPU(gecs::resource<Camera>,
                      gecs::resource<gecs::mut<RenderContext>>);

void UpdateGLTFModelTransform(
    gecs::querier<Transform, GLTFHandle, gecs::without<GlobalTransform>>,
    gecs::resource<gecs::mut<GLTFManager>>);

void BeginRender(gecs::resource<gecs::mut<rhi::Device>>,
                 gecs::resource<gecs::mut<RenderContext>>);

void EndRender(gecs::resource<gecs::mut<rhi::Device>>,
               gecs::resource<gecs::mut<RenderContext>>,
               gecs::resource<Window>);

void RenderSprite2D(
    gecs::resource<gecs::mut<rhi::Device>>,
    gecs::resource<gecs::mut<RenderContext>>, gecs::resource<gecs::mut<Camera>>,
    gecs::resource<TextureManager>, gecs::resource<Material2DManager>,
    gecs::querier<Transform, gecs::mut<Sprite>, SpriteMaterial>);

void RenderGLTFModel(gecs::resource<gecs::mut<RenderContext>>,
                     gecs::resource<gecs::mut<Camera>>,
                     gecs::resource<gecs::mut<rhi::Device>>,
                     gecs::resource<GLTFManager>,
                     gecs::querier<GLTFHandle, Transform>);

void BeginFrame(gecs::resource<gecs::mut<rhi::Device>>);

void EndFrame(gecs::resource<gecs::mut<rhi::Device>>,
                 gecs::resource<gecs::mut<RenderContext>>);
}  // namespace nickel