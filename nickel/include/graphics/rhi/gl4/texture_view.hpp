#pragma once

#include "graphics/rhi/gl4/glpch.hpp"
#include "graphics/rhi/impl/texture_view.hpp"
#include "graphics/rhi/texture.hpp"
#include "graphics/rhi/texture_view.hpp"


namespace nickel::rhi::gl4 {

class TextureImpl;

class TextureViewImpl : public rhi::TextureViewImpl {
public:
    explicit TextureViewImpl(TextureImpl&, const TextureView::Descriptor&);
};

}  // namespace nickel::rhi::gl4