#include "nickel/graphics/lowlevel/image.hpp"

#include "nickel/graphics/lowlevel/internal/image_impl.hpp"

namespace nickel::graphics {

ImageView Image::CreateView(const ImageView::Descriptor& desc) {
    return m_impl->CreateView(*this, desc);
}

SVector<uint32_t, 3> Image::Extent() const {
    return m_impl->Extent();
}

uint32_t Image::MipLevelCount() const {
    return m_impl->MipLevelCount();
}

}  // namespace nickel::graphics