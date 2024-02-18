#include "graphics/rhi/gl4/texture_view.hpp"
#include "graphics/rhi/gl4/texture.hpp"
#include "graphics/rhi/texture_view.hpp"

namespace nickel::rhi::gl4 {

TextureViewImpl::TextureViewImpl(TextureImpl& texture,
                                 const TextureView::Descriptor& desc)
    : rhi::TextureViewImpl{desc.format ? desc.format.value() : texture.Format(),
                           texture} {}

}  // namespace nickel::rhi::gl4
