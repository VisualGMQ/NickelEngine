#pragma once

#include "graphics/rhi/impl/texture_view.hpp"

namespace nickel::rhi::null {

class TextureImpl;

class TextureViewImpl : public rhi::TextureViewImpl {
public:
    explicit TextureViewImpl(TextureImpl&);
};

}