#include "graphics/rhi/gl4/texture.hpp"
#include "graphics/rhi/gl4/convert.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/texture_view.hpp"
#include "graphics/rhi/gl4/device.hpp"
#include "graphics/rhi/gl4/adapter.hpp"

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

TextureImpl::TextureImpl(DeviceImpl& device, const Texture::Descriptor& desc)
    : rhi::TextureImpl(desc), type_{getTypeFromDesc(desc)} {
    if (desc.format != TextureFormat::Presentation) {
        GL_CALL(glGenTextures(1, &id));
        Bind();
        GL_CALL(glTexImage2D(type_, 0, TextureFormat2GL(desc_.format),
                             desc.size.width, desc.size.height, 0,
                             TextureFormat2GL(desc_.format), GL_UNSIGNED_BYTE,
                             0));
    } else {
        id = device.swapchainTexture;
        desc_.dimension = TextureType::Dim2;
        desc_.mipmapLevelCount = 1;
        desc_.sampleCount = SampleCount::Count1; 
        desc_.usage = TextureUsage::RenderAttachment;
        desc_.viewFormat = TextureFormat::RGBA8_UNORM_SRGB;
        type_ = GL_TEXTURE_2D;
        int w, h;
        SDL_GetWindowSize(device.adapter->window, &w, &h);
        desc_.size.width = w;
        desc_.size.height = h;
        desc_.size.depthOrArrayLayers = 1;
    }
}

void TextureImpl::Bind(int slot) const {
    GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CALL(glBindTexture(type_, id));
}

void TextureImpl::Unbind() const {
    GL_CALL(glBindTexture(type_, 0));
}

TextureImpl::~TextureImpl() {
    if (desc_.format != TextureFormat::Presentation) {
        GL_CALL(glDeleteTextures(1, &id));
    }
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