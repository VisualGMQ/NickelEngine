#include "graphics/sprite.hpp"

namespace nickel {

Sprite Sprite::FromTexture(TextureHandle texture) {
    Sprite sprite;
    sprite.material = std::make_unique<Material2D>(
        texture, rhi::SamplerAddressMode::ClampToEdge,
        rhi::SamplerAddressMode::ClampToEdge, rhi::Filter::Linear,
        rhi::Filter::Linear);
    return sprite;
}

Sprite Sprite::FromRegion(TextureHandle texture, const cgmath::Rect& region) {
    Sprite sprite;
    sprite.region = region;
    sprite.customSize = region.size;
    sprite.material = std::make_unique<Material2D>(
        texture, rhi::SamplerAddressMode::ClampToEdge,
        rhi::SamplerAddressMode::ClampToEdge, rhi::Filter::Linear,
        rhi::Filter::Linear);
    return sprite;
}

Sprite Sprite::FromCustomSize(TextureHandle texture, const cgmath::Vec2& size) {
    Sprite sprite;
    sprite.customSize = size;
    sprite.material = std::make_unique<Material2D>(
        texture, rhi::SamplerAddressMode::ClampToEdge,
        rhi::SamplerAddressMode::ClampToEdge, rhi::Filter::Linear,
        rhi::Filter::Linear);
    return sprite;
}

}  // namespace nickel