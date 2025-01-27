#include "nickel/graphics/lowlevel/image.hpp"

#include "nickel/graphics/lowlevel/internal/image_impl.hpp"

namespace nickel::graphics {

Image::Image(ImageImpl* impl) : m_impl{impl} {}

Image::Image(const Image& o) : m_impl{o.m_impl} {
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

Image::Image(Image&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Image& Image::operator=(const Image& o) noexcept {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        if (m_impl) {
            m_impl->IncRefcount();
        }
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

void Image::Release() {
    if (m_impl) {
        m_impl->DecRefcount();
        m_impl = nullptr;
    }
}

ImageView Image::CreateView(const ImageView::Descriptor& desc) {
    return m_impl->CreateView(*this, desc);
}

Image::~Image() {
    Release();
}

SVector<uint32_t, 3> Image::Extent() const {
    return m_impl->Extent();
}

uint32_t Image::MipLevelCount() const {
    return m_impl->MipLevelCount();
}

Image::operator bool() const noexcept {
    return m_impl;
}

const ImageImpl& Image::Impl() const noexcept {
    return *m_impl;
}

ImageImpl& Image::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics