#pragma once

#include "common/asset.hpp"
#include "common/cgmath.hpp"
#include "common/filetype.hpp"
#include "common/handle.hpp"
#include "common/manager.hpp"
#include "rhi/rhi.hpp"

/**
 * @addtogroup resource-manager
 * @{
 */

namespace nickel {

class Texture;

using TextureHandle = Handle<Texture>;

class Texture final : public Asset {
public:
    friend class TextureManager;

    static Texture Null;

    explicit Texture(rhi::Device, const toml::table&);
    Texture(rhi::Device, const std::filesystem::path& filename,
        rhi::TextureFormat gpuFmt = rhi::TextureFormat::RGBA8_UNORM);
    Texture(rhi::Device, void*, int w, int h,
            rhi::TextureFormat gpuFmt = rhi::TextureFormat::RGBA8_UNORM);
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture(Texture&&) = default;
    Texture& operator=(Texture&&) = default;

    Texture& operator=(const Texture&) = delete;

    explicit operator bool() const { return texture_ && view_; }

    int Width() const { return w_; }

    int Height() const { return h_; }

    cgmath::Vec2 Size() const {
        return cgmath::Vec2{static_cast<float>(w_), static_cast<float>(h_)};
    }

    rhi::Texture Raw() const { return texture_; }
    rhi::TextureView View() const { return view_; }

    toml::table Save2Toml() const override;

private:
    rhi::Texture texture_;
    rhi::TextureView view_;
    int w_ = 0;
    int h_ = 0;

    rhi::Texture loadTexture(rhi::Device, void* data, uint32_t w, uint32_t h,
                             rhi::TextureFormat gpuFmt);
};

template <>
std::unique_ptr<Texture> LoadAssetFromMeta(const toml::table&);

class TextureManager final : public Manager<Texture> {
public:
    static FileType GetFileType() { return FileType::Image; }

    TextureHandle Load(
        const std::filesystem::path& filename,
        rhi::TextureFormat gpuFmt = rhi::TextureFormat::RGBA8_UNORM);
    TextureHandle Create(
        const std::filesystem::path& name, void* data, uint32_t w, uint32_t h,
        rhi::TextureFormat gpuFmt = rhi::TextureFormat::RGBA8_UNORM);
    TextureHandle LoadSVG(const std::filesystem::path& filename,
                          std::optional<cgmath::Vec2> size = std::nullopt);
    bool Replace(TextureHandle, const std::filesystem::path& filename,
                rhi::TextureFormat gpuFmt = rhi::TextureFormat::RGBA8_UNORM);
    std::unique_ptr<Texture> CreateSolitary(
        void* data, int w, int h,
        rhi::TextureFormat gpuFmt = rhi::TextureFormat::RGBA8_UNORM);
};

}  // namespace nickel
