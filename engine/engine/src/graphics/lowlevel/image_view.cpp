#include "nickel/graphics/lowlevel/image.hpp"
#include "nickel/graphics/lowlevel/internal/image_view_impl.hpp"

namespace nickel::graphics {

Image ImageView::GetImage() const {
    return m_impl->GetImage();
}

}  // namespace nickel::graphics