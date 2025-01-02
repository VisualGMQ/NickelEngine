#include "nickel/graphics/internal/image_view_impl.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/image_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

ImageViewImpl::ImageViewImpl(DeviceImpl& dev, const ImageView::Descriptor& desc)
    : m_device{dev.m_device} {
    VkImageViewCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.format = desc.format;
    ci.image = desc.image;
    ci.viewType = desc.viewType;
    ci.subresourceRange = desc.subresourceRange;

    VK_CALL(vkCreateImageView(dev.m_device, &ci, nullptr, &m_view));
}

ImageViewImpl::~ImageViewImpl() {
    vkDestroyImageView(m_device, m_view, nullptr);
}

}  // namespace nickel::graphics