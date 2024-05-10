#pragma once

#include "graphics/rhi/common.hpp"
#include "graphics/rhi/texture.hpp"

namespace nickel::rhi {

class TextureImpl;

class TextureViewImpl {
public:
    TextureViewImpl(TextureFormat, TextureImpl&);

    virtual ~TextureViewImpl() = default;

    enum TextureFormat Format() const;
    class Texture Texture() const;
    const Texture::Descriptor& TextureDescriptor() const;

private:
    enum TextureFormat format_;
    TextureImpl& texture_;
};

}