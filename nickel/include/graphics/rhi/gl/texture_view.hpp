#pragma once

#include "graphics/rhi/gl/glpch.hpp"
#include "graphics/rhi/impl/texture_view.hpp"
#include "graphics/rhi/texture.hpp"
#include "graphics/rhi/texture_view.hpp"


namespace nickel::rhi::gl {

class TextureImpl;

class TextureViewImpl : public rhi::TextureViewImpl {
public:
    TextureViewImpl(TextureImpl&, const TextureView::Descriptor&);
    ~TextureViewImpl();

    void Bind(uint32_t slot = 0) const;

    GLuint id = 0;
    GLenum type;
};

}  // namespace nickel::rhi::gl