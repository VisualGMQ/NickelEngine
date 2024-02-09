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
        Format format;
        Extent3D size;
        TextureType dimension = TextureType::Dim2;
        uint32_t mipmapLevelCount = 1;
        SampleCount sampleCount = SampleCount::Count1;
        TextureUsage usage;
        std::optional<Format> viewFormat;
    };

    Texture(AdapterImpl&, DeviceImpl&, const Descriptor&,
            const std::vector<uint32_t>& queueIndices);
    explicit Texture(TextureImpl*);
    void Destroy();

    TextureType Dimension() const;
    Extent3D Extent() const;
    enum Format Format() const;
    uint32_t MipLevelCount() const;
    SampleCount SampleCount() const;
    TextureUsage Usage() const;
    TextureView CreateView(const TextureView::Descriptor& = {});

    auto Impl() const { return impl_; }

private:
    TextureImpl* impl_{};
};

}  // namespace nickel::rhi