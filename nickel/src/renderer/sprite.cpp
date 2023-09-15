#include "renderer/sprite.hpp"

namespace nickel {

void SpriteBundle::RenderSprite(gecs::querier<SpriteBundle, Transform> sprites,
                                gecs::resource<gecs::mut<Renderer2D>> renderer,
                                gecs::resource<TextureManager> textureMgr) {
    for (auto& [_, sprite, transform] : sprites) {
        if (sprite.image && sprite.visiable) {
            Transform trans = transform;
            if (sprite.flip | Flip::Vertical) {
                trans.scale.x *= -1;
            }
            if (sprite.flip | Flip::Horizontal) {
                trans.scale.y *= -1;
            }
            renderer->DrawTexture(
                textureMgr->Get(sprite.image), sprite.sprite.region,
                sprite.sprite.customSize, sprite.sprite.color,
                cgmath::CreateTranslation(cgmath::Vec3{
                    sprite.sprite.anchor.x / sprite.sprite.region.w,
                    sprite.sprite.anchor.y / sprite.sprite.region.h, 0.0}) *
                    trans.ToMat());
        }
    }
}
}  // namespace nickel