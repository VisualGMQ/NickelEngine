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

    Material2D();
    Material2D(const toml::table& tbl);
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

    toml::table Save2Toml() const override;

    operator bool() const noexcept;

private:
    TextureHandle texture_;
    rhi::Sampler::Descriptor samplerDesc_;
    rhi::BindGroup bindGroup_;

    rhi::BindGroup createBindGroup(rhi::TextureView);
};

template <>
std::unique_ptr<Material2D> LoadAssetFromMetaTable(const toml::table& tbl);

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

class Material2DManager : public Manager<Material2D> {
public:
    Material2DHandle Create(
        TextureHandle handle,
        rhi::SamplerAddressMode u = rhi::SamplerAddressMode::Repeat,
        rhi::SamplerAddressMode v = rhi::SamplerAddressMode::Repeat,
        rhi::Filter min = rhi::Filter::Linear,
        rhi::Filter mag = rhi::Filter::Linear);

    Material2DHandle Load(const std::filesystem::path&);

    static FileType GetFileType() { return FileType::Material2D; }
};

using Material3DHandle = Handle<Material3D>;

// class Material3DManager : public Manager<Material3D> {
// public:
//     Material3DHandle Create(TextureHandle handle, rhi::SamplerAddressMode u,
//                             rhi::SamplerAddressMode v, rhi::Filter min,
//                             rhi::Filter mag);
// };

}  // namespace nickel