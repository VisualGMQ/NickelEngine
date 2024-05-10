#include "graphics/rhi/null/texture_view.hpp"
#include "graphics/rhi/null/texture.hpp"

namespace nickel::rhi::null {

TextureViewImpl::TextureViewImpl(TextureImpl& impl)
    : rhi::TextureViewImpl{TextureFormat::Undefined, impl} {}
}