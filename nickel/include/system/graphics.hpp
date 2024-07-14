#pragma once

#include "common/transform.hpp"
#include "graphics/camera.hpp"
#include "graphics/context.hpp"
#include "graphics/gltf.hpp"
#include "graphics/sprite.hpp"
#include "video/window.hpp"


namespace nickel {

void UpdateCamera2GPU(gecs::resource<Camera>);

void UpdateGLTFModelTransform(gecs::querier<Transform, GLTFHandle, gecs::without<GlobalTransform>>);

void BeginRender();

void EndRender(gecs::resource<Window>);

void RenderSprite2D(
    gecs::resource<gecs::mut<Camera>>,
    gecs::querier<Transform, gecs::mut<Sprite>, SpriteMaterial>);

void RenderGLTFModel(gecs::resource<gecs::mut<Camera>>,
                     gecs::querier<GLTFHandle, Transform>);

void BeginFrame();

void EndFrame();
}  // namespace nickel