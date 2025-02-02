#include "nickel/graphics/texture.hpp"

#include "nickel/graphics/internal/texture_manager_impl.hpp"
#include "nickel/graphics/lowlevel/common.hpp"
#include "nickel/graphics/lowlevel/device.hpp"
#include "nickel/graphics/lowlevel/image.hpp"
#include "nickel/graphics/lowlevel/internal/image_impl.hpp"
#include "nickel/graphics/lowlevel/internal/image_view_impl.hpp"

namespace nickel::graphics {

Texture::Texture(TextureImpl* impl)
    : m_impl{impl} {
}

TextureImpl& Texture::GetImpl() noexcept {
    return *m_impl;
}

const TextureImpl& Texture::GetImpl() const noexcept {
    return *m_impl;
}

Texture::operator bool() const noexcept {
    return m_impl;
}
}
