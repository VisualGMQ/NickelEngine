#include "graphics/rhi/impl/texture_view.hpp"
#include "graphics/rhi/vk/texture.hpp"

namespace nickel::rhi {

TextureViewImpl::TextureViewImpl(enum TextureFormat format, TextureImpl& texture)
    : format_{format}, texture_{texture} {}

const Texture::Descriptor& TextureViewImpl::TextureDescriptor() const {
    return texture_.Descriptor();
}

enum TextureFormat TextureViewImpl::Format() const {
    return format_;
}

class Texture TextureViewImpl::Texture() const {
    return (class Texture){&texture_};
}

}  // namespace nickel::rhi