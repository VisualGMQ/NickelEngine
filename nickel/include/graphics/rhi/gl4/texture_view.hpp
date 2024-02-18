#pragma once

#include "glad/glad.h"
#include "graphics/rhi/impl/texture_view.hpp"
#include "graphics/rhi/texture.hpp"
#include "graphics/rhi/texture_view.hpp"


namespace nickel::rhi::gl4 {

class TextureImpl;

class TextureViewImpl : public rhi::TextureViewImpl {
public:
    explicit TextureViewImpl(TextureImpl&, const TextureView::Descriptor&);

    TextureImpl* texture{};
};

}  // namespace nickel::rhi::gl4