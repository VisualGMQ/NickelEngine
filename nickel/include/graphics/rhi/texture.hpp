#pragma once

#include "graphics/rhi/common.hpp"
#include "graphics/rhi/texture_view.hpp"
#include "stdpch.hpp"

namespace nickel::rhi {

class TextureImpl;
class DeviceImpl;
class AdapterImpl;

class Texture {
public:
    struct Descriptor final {
        TextureFormat format;
        Extent3D size;
        TextureType dimension = TextureType::Dim2;
        uint32_t mipmapLevelCount = 1;
        SampleCount sampleCount = SampleCount::Count1;
        Flags<TextureUsage> usage;
        std::optional<TextureFormat> viewFormat;
        Flags<TextureFlagBits> flags = TextureFlagBits::None;
    };

    Texture() = default;
    Texture(AdapterImpl&, DeviceImpl&, const Descriptor&,
            const std::vector<uint32_t>& queueIndices);
    Texture(Texture&& o) noexcept { swap(o, *this); }
    Texture(const Texture& o) = default;
    Texture& operator=(const Texture& o) = default;

    Texture& operator=(Texture&& o) noexcept {
        if (&o != this) swap(o, *this);
        return *this;
    }
    explicit Texture(TextureImpl*);
    void Destroy();

    operator bool() const { return impl_; }

    TextureType Dimension() const;
    Extent3D Extent() const;
    enum TextureFormat Format() const;
    uint32_t MipLevelCount() const;
    enum SampleCount SampleCount() const;
    TextureUsage Usage() const;
    TextureView CreateView(const TextureView::Descriptor& = {});
    const Descriptor& GetDescriptor() const;

    auto Impl() const { return impl_; }

private:
    TextureImpl* impl_{};

    friend void swap(Texture& o1, Texture& o2) noexcept {
        using std::swap;

        swap(o1.impl_, o2.impl_);
    }
};

}  // namespace nickel::rhi