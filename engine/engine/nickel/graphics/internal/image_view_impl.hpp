#pragma once
#include "nickel/graphics/image_view.hpp"
#include "refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;
class ImageImpl;

class ImageViewImpl : public RefCountable {
public:
    ImageViewImpl(DeviceImpl&, const ImageView::Descriptor&);
    ~ImageViewImpl();

    VkImageView m_view;

private:
    VkDevice m_device;
};

}  // namespace nickel::graphics