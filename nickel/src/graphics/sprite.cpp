#include "graphics/sprite.hpp"
#include "common/profile.hpp"

namespace nickel {

Sprite Sprite::FromTexture(TextureHandle texture) {
    Sprite sprite;
    sprite.texture = texture;
    return sprite;
}

Sprite Sprite::FromRegion(TextureHandle texture, const cgmath::Rect& region) {
    Sprite sprite;
    sprite.region = region;
    sprite.customSize = region.size;
    sprite.texture = texture;
    return sprite;
}

Sprite Sprite::FromCustomSize(TextureHandle texture, const cgmath::Vec2& size) {
    Sprite sprite;
    sprite.customSize = size;
    sprite.texture = texture;
    return sprite;
}

}  // namespace nickel