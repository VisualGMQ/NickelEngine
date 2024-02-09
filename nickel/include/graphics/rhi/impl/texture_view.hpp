#pragma once

#include "graphics/rhi/common.hpp"
#include "graphics/rhi/texture.hpp"

namespace nickel::rhi {

class TextureImpl;

class TextureViewImpl {
public:
    TextureViewImpl(Format, TextureImpl&);

    virtual ~TextureViewImpl() = default;

    enum Format Format() const;
    class Texture Texture() const;
    const Texture::Descriptor& TextureDescriptor() const;

private:
    enum Format format_;
    TextureImpl& texture_;
};

}