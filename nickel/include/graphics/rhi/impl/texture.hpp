#pragma once

#include "graphics/rhi/common.hpp"
#include "graphics/rhi/texture.hpp"
#include "graphics/rhi/texture_view.hpp"

namespace nickel::rhi {

class TextureImpl {
public:
    explicit TextureImpl(const Texture::Descriptor& desc);
    virtual ~TextureImpl() = default;

    Extent3D Extent() const;
    enum TextureFormat Format() const;
    uint32_t MipLevelCount() const;
    enum SampleCount SampleCount() const;
    TextureUsage Usage() const;
    TextureType Dimension() const;
    const Texture::Descriptor& Descriptor() const;

    virtual TextureView CreateView(const TextureView::Descriptor& = {}) = 0;

protected:
    Texture::Descriptor desc_;
};

}