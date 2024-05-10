#include "graphics/sprite.hpp"

namespace nickel {

Sprite Sprite::FromRegion(const cgmath::Rect& region) {
    Sprite sprite;
    sprite.region = region;
    sprite.customSize = region.size;
    return sprite;
}

Sprite Sprite::FromCustomSize(const cgmath::Vec2& size) {
    Sprite sprite;
    sprite.customSize = size;
    return sprite;
}

Sprite::~Sprite() {
    if (slot) {
        ECS::Instance()
            .World()
            .res_mut<RenderContext>()
            ->ctx2D->ReuseVertexSlot(slot.value());
    }
}

}  // namespace nickel