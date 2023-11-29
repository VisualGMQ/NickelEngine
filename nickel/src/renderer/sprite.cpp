#include "renderer/sprite.hpp"

namespace nickel {

void RenderSprite(gecs::querier<Sprite, Transform> sprites,
                                gecs::resource<gecs::mut<Renderer2D>> renderer,
                                gecs::resource<TextureManager> textureMgr,
                                gecs::resource<Camera> camera,
                                gecs::resource<gecs::mut<RenderContext>> renderCtx) {
    renderer->BeginRender(camera.get());
    for (auto&& [_, sprite, transform] : sprites) {
        if (sprite.texture && sprite.visiable) {
            renderCtx->depthBias += renderCtx->depthBiasStep;
            float depth = sprite.zIndex + renderCtx->depthBias;

            Transform trans = transform;
            if (sprite.flip & Flip::Vertical) {
                trans.scale.y *= -1;
            }
            if (sprite.flip & Flip::Horizontal) {
                trans.scale.x *= -1;
            }
            auto& texture = textureMgr->Get(sprite.texture);
            cgmath::Rect region =
                sprite.region
                    ? sprite.region.value()
                    : cgmath::Rect{0, 0, static_cast<float>(texture.Width()),
                                   static_cast<float>(texture.Height())};
            cgmath::Vec2 customSize = sprite.customSize
                                          ? sprite.customSize.value()
                                          : texture.Size();
            renderer->DrawTexture(textureMgr->Get(sprite.texture), region,
                                  customSize, sprite.color,
                                  sprite.anchor,
                                  depth,
                                  trans.ToMat());
        }
    }
    renderer->EndRender();
}

}  // namespace nickel