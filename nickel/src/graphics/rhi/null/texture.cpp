#include "graphics/rhi/null/texture.hpp"
#include "graphics/rhi/texture_view.hpp"
#include "graphics/rhi/null/texture_view.hpp"

namespace nickel::rhi::null {

TextureImpl::TextureImpl() : rhi::TextureImpl{{}} {}

TextureView TextureImpl::CreateView(const TextureView::Descriptor&) {
    return TextureView{new TextureViewImpl{*this}};
}

}  // namespace nickel::rhi::null