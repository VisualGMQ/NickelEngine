#include "graphics/rhi/gl4/texture_view.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/texture.hpp"
#include "graphics/rhi/texture_view.hpp"

namespace nickel::rhi::gl4 {

TextureViewImpl::TextureViewImpl(TextureImpl& texture,
                                 const TextureView::Descriptor& desc)
    : rhi::TextureViewImpl{desc.format.value_or(texture.Format()), texture} {
    if (texture.Format() != TextureFormat::Presentation) {
        texture.Bind();
        GL_CALL(glGenTextures(1, &id_));
        type_ = desc.dimension ? TextureViewType2GL(desc.dimension.value())
                        : texture.Type();
        GL_CALL(glTextureView(
            id_, type_, texture.id,
            TextureFormat2GLInternal(desc.format.value_or(texture.Format())), 0,
            1, 0, type_ == GL_TEXTURE_CUBE_MAP ? 6 : 1));
    } else {
        id_ = texture.id;
    }
}

TextureViewImpl::~TextureViewImpl() {
    if (id_ != 0 && Format() != TextureFormat::Presentation) {
        GL_CALL(glDeleteTextures(1, &id_));
    }
}

void TextureViewImpl::Bind(uint32_t slot) const {
    GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CALL(glBindTexture(type_, id_));
}

}  // namespace nickel::rhi::gl4
