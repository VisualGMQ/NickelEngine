#pragma once

#include "graphics/texture.hpp"
#include "rhi/rhi.hpp"

namespace nickel {

/**
 * @brief component 
 */
class Material2D: public Asset {
public:
    static Material2D Null;

    Material2D() = default;
    Material2D(
        TextureHandle handle,
        rhi::SamplerAddressMode u = rhi::SamplerAddressMode::Repeat,
        rhi::SamplerAddressMode v = rhi::SamplerAddressMode::Repeat,
        rhi::Filter min = rhi::Filter::Linear,
        rhi::Filter mag = rhi::Filter::Linear);
    Material2D(Material2D&&) = default;
    Material2D& operator=(Material2D&&) = default;
    Material2D(const Material2D&) = delete;
    Material2D& operator=(const Material2D&) = delete;
    ~Material2D();

    rhi::Sampler::Descriptor& GetSamplerDesc();
    const rhi::Sampler::Descriptor& GetSamplerDesc() const;
    TextureHandle GetTexture() const;
    rhi::BindGroup GetBindGroup() const;

    bool ChangeTexture(TextureHandle handle);
    bool ChangeSampler(rhi::SamplerAddressMode u, rhi::SamplerAddressMode v,
                       rhi::Filter min, rhi::Filter mag);

    bool Load(const toml::table&) override;
    bool Save(toml::table&) const override;

    operator bool() const noexcept;

private:
    TextureHandle texture_;
    rhi::Sampler::Descriptor samplerDesc_;
    rhi::BindGroup bindGroup_;

    rhi::BindGroup createBindGroup(bool supportSeparateSampler,
                                   rhi::TextureView);
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

/**
 * @brief component
 */
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

using Material2DHandle = Handle<Material2D>;
using Material3DHandle = Handle<Material3D>;

}  // namespace nickel