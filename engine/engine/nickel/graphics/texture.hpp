#pragma once
#include "nickel/common/impl_wrapper.hpp"

namespace nickel::graphics {

class TextureImpl;

class Texture : public ImplWrapper<TextureImpl> {
public:
    using ImplWrapper::ImplWrapper;
};

}  // namespace nickel::graphics