#include "nickel/graphics/lowlevel/internal/image_view_impl.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/image_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

ImageViewImpl::ImageViewImpl(DeviceImpl& dev, const Image& image,
                             const ImageView::Descriptor& desc)
    : m_device{dev} {
    VkImageViewCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.format = Format2Vk(desc.m_format);
    ci.image = image.GetImpl()->m_image;
    ci.viewType = ImageViewType2Vk(desc.m_view_type);
    VkImageSubresourceRange range{};
    range.aspectMask = ImageAspect2Vk(desc.m_subresource_range.m_aspect_mask);
    range.layerCount = desc.m_subresource_range.m_layer_count;
    range.levelCount = desc.m_subresource_range.m_level_count;
    range.baseArrayLayer = desc.m_subresource_range.m_base_array_layer;
    range.baseMipLevel = desc.m_subresource_range.m_base_mipLevel;
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
        m_device.m_image_view_allocator.MarkAsGarbage(this);
    }
}

Image ImageViewImpl::GetImage() const {
    return Image{m_image};
}

}  // namespace nickel::graphics