#include "graphics/rhi/gl4/texture.hpp"
#include "graphics/rhi/gl4/convert.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/texture_view.hpp"

namespace nickel::rhi::gl4 {

GLenum getTypeFromDesc(const Texture::Descriptor& desc) {
    if (desc.dimension == TextureType::Dim1) {
        if (desc.size.depthOrArrayLayers > 1) {
            return GL_TEXTURE_1D_ARRAY;
        } else {
            return GL_TEXTURE_1D;
        }
    }

    if (desc.dimension == TextureType::Dim2) {
        if (desc.size.depthOrArrayLayers > 1) {
            return GL_TEXTURE_2D_ARRAY;
        } else {
            return GL_TEXTURE_2D;
        }
    }

    if (desc.dimension == TextureType::Dim3) {
        return GL_TEXTURE_3D;
    }

    return GL_TEXTURE_2D;
}

TextureImpl::TextureImpl(const Texture::Descriptor& desc)
    : rhi::TextureImpl(desc), type_{getTypeFromDesc(desc)} {
    GL_CALL(glGenTextures(1, &id));
}

void TextureImpl::Bind(int slot) const {
    GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CALL(glBindTexture(static_cast<GLenum>(type_), id));
}

void TextureImpl::Unbind() const {
    GL_CALL(glBindTexture(static_cast<GLenum>(type_), 0));
}

TextureImpl::~TextureImpl() {
    GL_CALL(glDeleteTextures(1, &id));
}

TextureView TextureImpl::CreateView(const TextureView::Descriptor& desc) {
    return TextureView{
        new TextureViewImpl{*this, desc}
    };
}

GLenum TextureImpl::Type() const {
    return type_;
}

}  // namespace nickel::rhi::gl4