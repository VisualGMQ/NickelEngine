#include "nickel/graphics/internal/image_impl.hpp"

#include "nickel/graphics/internal/adapter_impl.hpp"
#include "nickel/graphics/internal/common.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/enum_convert.hpp"
#include "nickel/graphics/internal/memory_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

ImageImpl::ImageImpl(const AdapterImpl& adapter, DeviceImpl& dev,
                     const Image::Descriptor& desc)
    : m_device{dev} {
    createImage(desc, dev);
    allocMem(adapter.m_phyDevice);

    for (int i = 0; i < desc.extent.l; i++) {
        m_layouts.push_back(desc.initialLayout);
    }

    VK_CALL(
        vkBindImageMemory(m_device.m_device, m_image, m_memory->m_memory, 0));
}

void ImageImpl::createImage(const Image::Descriptor& desc, DeviceImpl& dev) {
    VkImageCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ci.imageType = ImageType2Vk(desc.imageType);
    ci.format = Format2Vk(desc.format);
    ci.extent = VkExtent3D{desc.extent.w, desc.extent.h, desc.extent.l};
    ci.mipLevels = desc.mipLevels;
    ci.arrayLayers = desc.arrayLayers;
    ci.samples =
        static_cast<VkSampleCountFlagBits>(SampleCount2Vk(desc.samples));
    ci.tiling = ImageTiling2Vk(desc.tiling);
    ci.usage = ImageUsage2Vk(desc.usage);
    ci.sharingMode = SharingMode2Vk(desc.sharingMode);
    ci.initialLayout = ImageLayout2Vk(desc.initialLayout);
    auto indices = dev.m_queue_indices.GetIndices();
    ci.pQueueFamilyIndices = indices.data();
    ci.queueFamilyIndexCount = indices.size();
    if (indices.size() > 1) {
        ci.sharingMode = VK_SHARING_MODE_CONCURRENT;
    } else {
        ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    m_create_info = ci;

    VK_CALL(vkCreateImage(dev.m_device, &ci, nullptr, &m_image));
}

void ImageImpl::allocMem(VkPhysicalDevice phyDevice) {
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(m_device.m_device, m_image, &requirements);
    auto index = FindMemoryType(phyDevice, requirements,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (!index) {
        LOGE("allocate image memory failed: no satisfied memory type "
             "(DeviceLocal)");
    } else {
        m_memory =
            new MemoryImpl{m_device, requirements.size, index.value()};
    }
}

ImageImpl::~ImageImpl() {
    vkDestroyImage(m_device.m_device, m_image, nullptr);
    delete m_memory;
}

VkImageType ImageImpl::Type() const {
    return m_create_info.imageType;
}

SVector<uint32_t, 3> ImageImpl::Extent() const {
    SVector<uint32_t, 3> extent{m_create_info.extent.width,
                                m_create_info.extent.height,
                                m_create_info.extent.depth};
    return extent;
}

VkFormat ImageImpl::Format() const {
    return m_create_info.format;
}

uint32_t ImageImpl::MipLevelCount() const {
    return m_create_info.mipLevels;
}

VkSampleCountFlags ImageImpl::SampleCount() const {
    return m_create_info.samples;
}

Flags<VkImageUsageFlagBits> ImageImpl::Usage() const {
    return m_create_info.usage;
}

ImageView ImageImpl::CreateView(const Image& image,
                                const ImageView::Descriptor& desc) {
    return m_device.CreateImageView(image, desc);
}

void ImageImpl::PendingDelete() {
    m_device.m_pending_delete_images.push_back(this);
}

}  // namespace nickel::graphics