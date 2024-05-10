#include "graphics/rhi/gl4/texture_view.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/texture.hpp"
#include "graphics/rhi/texture_view.hpp"

namespace nickel::rhi::gl4 {

TextureViewImpl::TextureViewImpl(TextureImpl& texture,
                                 const TextureView::Descriptor& desc)
    : rhi::TextureViewImpl{desc.format.value_or(texture.Format()), texture} {
#ifdef NICKEL_HAS_GL4
    if (texture.Format() != TextureFormat::Presentation) {
        texture.Bind();
        GL_CALL(glGenTextures(1, &id));
        type = desc.dimension ? TextureViewType2GL(desc.dimension.value())
                              : texture.Type();
        GL_CALL(glTextureView(
            id, type, texture.id,
            TextureFormat2GLInternal(desc.format.value_or(texture.Format())), 0,
            1, 0, type == GL_TEXTURE_CUBE_MAP ? 6 : 1));
    } else {
        type = GL_TEXTURE_2D;
        id = texture.id;
    }
#else
    if (texture.Format() != TextureFormat::Presentation) {
        type = desc.dimension ? TextureViewType2GL(desc.dimension.value())
                                : texture.Type();
    } else {
        type = GL_TEXTURE_2D;
    }
    id = texture.id;
#endif
}

TextureViewImpl::~TextureViewImpl() {
#ifdef NICKEL_HAS_GL4
    if (id != 0 && Format() != TextureFormat::Presentation) {
        GL_CALL(glDeleteTextures(1, &id));
    }
#endif
}

void TextureViewImpl::Bind(uint32_t slot) const {
    GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CALL(glBindTexture(type, id));
}

}  // namespace nickel::rhi::gl4
