#pragma once
#include "nickel/graphics/image.hpp"
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;
class ImageImpl;

class ImageViewImpl : public RefCountable {
public:
    ImageViewImpl(DeviceImpl&, const Image& image, const ImageView::Descriptor&);

    // only for swapchain image view hack
    ImageViewImpl(VkDevice, VkImageView);

    ~ImageViewImpl();

    Image GetImage() const;

    VkImageView m_view;
    Image m_image;

private:
    VkDevice m_device;
};

}  // namespace nickel::graphics