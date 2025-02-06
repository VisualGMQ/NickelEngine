#pragma once
#include "nickel/graphics/lowlevel/image.hpp"
#include "nickel/common/memory/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;
class ImageImpl;

class ImageViewImpl : public RefCountable {
public:
    ImageViewImpl(DeviceImpl&, const Image& image, const ImageView::Descriptor&);

    // only for swapchain image view hack
    ImageViewImpl(DeviceImpl&, VkImageView);
    ImageViewImpl(const ImageViewImpl&) = delete;
    ImageViewImpl(ImageViewImpl&&) = delete;
    ImageViewImpl& operator=(const ImageViewImpl&) = delete;
    ImageViewImpl& operator=(ImageViewImpl&&) = delete;

    ~ImageViewImpl();

    void DecRefcount() override;

    Image GetImage() const;

    VkImageView m_view = VK_NULL_HANDLE;
    Image m_image;

private:
    DeviceImpl& m_device;
};

}  // namespace nickel::graphics