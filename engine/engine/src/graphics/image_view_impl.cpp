#include "nickel/graphics/internal/image_view_impl.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/enum_convert.hpp"
#include "nickel/graphics/internal/image_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

ImageViewImpl::ImageViewImpl(DeviceImpl& dev, const Image& image,
                             const ImageView::Descriptor& desc)
    : m_device{dev} {
    VkImageViewCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.format = Format2Vk(desc.format);
    ci.image = image.Impl().m_image;
    ci.viewType = ImageViewType2Vk(desc.viewType);
    VkImageSubresourceRange range{};
    range.aspectMask = ImageAspect2Vk(desc.subresourceRange.aspectMask);
    range.layerCount = desc.subresourceRange.layerCount;
    range.levelCount = desc.subresourceRange.levelCount;
    range.baseArrayLayer = desc.subresourceRange.baseArrayLayer;
    range.baseMipLevel = desc.subresourceRange.baseMipLevel;
    ci.subresourceRange = range;

    VK_CALL(vkCreateImageView(dev.m_device, &ci, nullptr, &m_view));

    m_image = image;
}

ImageViewImpl::ImageViewImpl(DeviceImpl& device, VkImageView view)
    : m_device{device}, m_view{view} {}

ImageViewImpl::~ImageViewImpl() {
    vkDestroyImageView(m_device.m_device, m_view, nullptr);
}

void ImageViewImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_device.m_pending_delete_image_views.push_back(this);
    }
}

Image ImageViewImpl::GetImage() const {
    return Image{m_image};
}

}  // namespace nickel::graphics