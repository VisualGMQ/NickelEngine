#pragma once

#include "graphics/texture.hpp"
#include "rhi/rhi.hpp"

namespace nickel {

class Material2D {
public:
    Material2D();
    explicit Material2D(TextureHandle handle, rhi::SamplerAddressMode u,
                        rhi::SamplerAddressMode v, rhi::Filter min,
                        rhi::Filter mag);
    Material2D(Material2D&&) = default;
    Material2D& operator=(Material2D&&) = default;
    Material2D(const Material2D&) = delete;
    Material2D& operator=(const Material2D&) = delete;
    ~Material2D();

    rhi::Sampler::Descriptor& GetSamplerDesc();
    const rhi::Sampler::Descriptor& GetSamplerDesc() const;
    TextureHandle GetTexture() const;
    rhi::BindGroup GetBindGroup() const;

    void ChangeTexture(TextureHandle handle);
    void ChangeSampler(rhi::SamplerAddressMode u, rhi::SamplerAddressMode v,
                       rhi::Filter min, rhi::Filter mag);

private:
    TextureHandle texture_;
    rhi::Sampler::Descriptor samplerDesc_;
    rhi::BindGroup bindGroup_;

    rhi::BindGroup createBindGroup(rhi::TextureView);
};

struct BufferView {
    uint32_t offset{};
    uint64_t size{};
    uint32_t count{};
};

struct PBRParameters {
    nickel::cgmath::Vec4 baseColor;
    float metalness = 1.0f;
    float roughness = 1.0f;
};

struct Material3D final {
    struct TextureInfo {
        TextureHandle texture;
        std::optional<uint32_t> sampler;
    };

    BufferView pbrParameters;
    std::optional<TextureInfo> basicTexture;
    std::optional<TextureInfo> normalTexture;
    std::optional<TextureInfo> metalicRoughnessTexture;
    std::optional<TextureInfo> occlusionTexture;
    rhi::BindGroup bindGroup;

    Material3D() = default;
    Material3D(Material3D&&) = default;
    Material3D& operator=(Material3D&&) = default;
    Material3D(const Material3D&) = delete;
    Material3D& operator=(const Material3D&) = delete;

    ~Material3D();
};

struct TextureBundle {
    rhi::Texture texture;
    rhi::TextureView view;

    ~TextureBundle();
};


}  // namespace nickel