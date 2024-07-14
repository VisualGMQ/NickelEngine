#pragma once

#include "common/asset.hpp"
#include "common/cgmath.hpp"
#include "common/handle.hpp"
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
    static Texture Null;

    Texture(void*, int w, int h,
            rhi::TextureFormat gpuFmt = rhi::TextureFormat::RGBA8_UNORM,
            rhi::Flags<rhi::TextureUsage> usage =
                rhi::Flags(rhi::TextureUsage::TextureBinding) |
                rhi::TextureUsage::CopyDst);
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

    rhi::Texture RawTexture() const { return texture_; }

    rhi::TextureView View() const { return view_; }

    bool Load(const std::filesystem::path&) override;
    bool Load(const toml::table&) override;
    bool Save(toml::table&) const override;

private:
    rhi::Texture texture_;
    rhi::TextureView view_;
    int w_ = 0;
    int h_ = 0;

    rhi::Texture createTexture(rhi::Device, void* data, uint32_t w, uint32_t h,
                               rhi::TextureFormat gpuFmt,
                               rhi::Flags<rhi::TextureUsage> usage);
};

}  // namespace nickel
