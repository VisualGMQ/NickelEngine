#include "nickel/graphics/image_view.hpp"
#include "nickel/graphics/internal/image_view_impl.hpp"

namespace nickel::graphics {

ImageView::ImageView(ImageViewImpl* impl) : m_impl{impl} {}

ImageView::ImageView(const ImageView& o) : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

ImageView::ImageView(ImageView&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

ImageView& ImageView::operator=(const ImageView& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
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
    m_impl->DecRefcount();
}

const ImageViewImpl& ImageView::Impl() const noexcept {
    return *m_impl;
}

ImageViewImpl& ImageView::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics