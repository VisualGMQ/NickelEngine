#pragma once

#include "graphics/rhi/impl/texture.hpp"

namespace nickel::rhi::null {

class TextureImpl : public rhi::TextureImpl {
public:
    TextureImpl();
    TextureView CreateView(const TextureView::Descriptor&) override;
};

}  // namespace nickel::rhi::null