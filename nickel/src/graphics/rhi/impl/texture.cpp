#include "graphics/rhi/impl/texture.hpp"

namespace nickel::rhi {

TextureImpl::TextureImpl(const Texture::Descriptor& desc) : desc_{desc} {}

Extent3D TextureImpl::Extent() const {
    return desc_.size;
}

enum Format TextureImpl::Format() const {
    return desc_.format;
}

uint32_t TextureImpl::MipLevelCount() const {
    return desc_.mipmapLevelCount;
}

SampleCount TextureImpl::SampleCount() const {
    return desc_.sampleCount;
}

TextureUsage TextureImpl::Usage() const {
    return desc_.usage;
}

TextureType TextureImpl::Dimension() const {
    return desc_.dimension;
}

const Texture::Descriptor& TextureImpl::Descriptor() const {
    return desc_;
}

}