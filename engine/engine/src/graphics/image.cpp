#include "nickel/graphics/image.hpp"

#include "nickel/graphics/internal/image_impl.hpp"

namespace nickel::graphics {

Image::Image(ImageImpl* impl) : m_impl{impl} {}

Image::Image(const Image& o) : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

Image::Image(Image&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Image& Image::operator=(const Image& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

Image& Image::operator=(Image&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

Image::~Image() {
    m_impl->DecRefcount();
}


}