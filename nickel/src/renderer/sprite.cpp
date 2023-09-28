#include "renderer/sprite.hpp"

namespace nickel {

void SpriteBundle::RenderSprite(gecs::querier<SpriteBundle, Transform> sprites,
                                gecs::resource<gecs::mut<Renderer2D>> renderer,
                                gecs::resource<TextureManager> textureMgr) {
    for (auto&& [_, sprite, transform] : sprites) {
        if (sprite.image && sprite.visiable) {
            Transform trans = transform;
            if (sprite.flip & Flip::Vertical) {
                trans.scale.y *= -1;
            }
            if (sprite.flip & Flip::Horizontal) {
                trans.scale.x *= -1;
            }
            auto& texture = textureMgr->Get(sprite.image);
            cgmath::Rect region =
                sprite.sprite.region
                    ? sprite.sprite.region.value()
                    : cgmath::Rect{0, 0, static_cast<float>(texture.Width()),
                                   static_cast<float>(texture.Height())};
            cgmath::Vec2 customSize = sprite.sprite.customSize
                                          ? sprite.sprite.customSize.value()
                                          : texture.Size();
            renderer->DrawTexture(textureMgr->Get(sprite.image), region,
                                  customSize, sprite.sprite.color,
                                  cgmath::CreateTranslation(cgmath::Vec3{
                                      sprite.sprite.anchor.x / region.w,
                                      sprite.sprite.anchor.y / region.h, 0.0}) *
                                      trans.ToMat());
        }
    }
}

}  // namespace nickel