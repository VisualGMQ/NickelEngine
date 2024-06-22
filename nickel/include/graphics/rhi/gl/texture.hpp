#pragma once
#include "graphics/rhi/texture.hpp"
#include "graphics/rhi/impl/texture.hpp"
#include "graphics/rhi/gl/convert.hpp"

namespace nickel::rhi::gl {

class DeviceImpl;

class TextureImpl: public rhi::TextureImpl {
public:
    TextureImpl(DeviceImpl&, const Texture::Descriptor&);
    TextureImpl(GLuint id, GLenum type);
    ~TextureImpl();

    void Bind(int slot = 0) const;
    void Unbind() const;

    TextureView CreateView(const TextureView::Descriptor&) override;

    GLenum Type() const;

    GLuint id;

private:
    GLenum type_;
};

}