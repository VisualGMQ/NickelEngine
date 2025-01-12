#include "nickel/graphics/image.hpp"
#include "nickel/graphics/internal/image_view_impl.hpp"

namespace nickel::graphics {

ImageView::ImageView(ImageViewImpl* impl) : m_impl{impl} {}

ImageView::ImageView(const ImageView& o) : m_impl{o.m_impl} {
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

ImageView::ImageView(ImageView&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

Image ImageView::GetImage() const {
    return m_impl->GetImage();
}

ImageView& ImageView::operator=(const ImageView& o) noexcept {
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

ImageView& ImageView::operator=(ImageView&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

ImageView::~ImageView() {
    Release();
}

ImageView::operator bool() const noexcept {
    return m_impl;
}

const ImageViewImpl& ImageView::Impl() const noexcept {
    return *m_impl;
}

ImageViewImpl& ImageView::Impl() noexcept {
    return *m_impl;
}

void ImageView::Release() {
    if (m_impl) {
        m_impl->Release();
        m_impl->DecRefcount();
        if (m_impl->Refcount() == 0) {
            m_impl->PendingDelete();
        }
        m_impl = nullptr;
    }
}

}  // namespace nickel::graphics