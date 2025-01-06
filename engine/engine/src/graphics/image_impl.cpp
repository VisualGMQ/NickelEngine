#include "nickel/graphics/internal/image_impl.hpp"

#include "nickel/graphics/internal/adapter_impl.hpp"
#include "nickel/graphics/internal/common.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/memory_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

ImageImpl::ImageImpl(const AdapterImpl& adapter, DeviceImpl& dev,
                     const Image::Descriptor& desc)
    : m_device{dev} {
    createImage(desc, dev);
    allocMem(adapter.m_phyDevice);

    for (int i = 0; i < desc.extent.depth; i++) {
        m_layouts.push_back(desc.initialLayout);
    }

    VK_CALL(
        vkBindImageMemory(m_device.m_device, m_image, m_memory->m_memory, 0));
}

void ImageImpl::createImage(const Image::Descriptor& desc, DeviceImpl& dev) {
    VkImageCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ci.imageType = desc.imageType;
    ci.format = desc.format;
    ci.extent = desc.extent;
    ci.mipLevels = desc.mipLevels;
    ci.arrayLayers = desc.arrayLayers;
    ci.samples = desc.samples;
    ci.tiling = desc.tiling;
    ci.usage = desc.usage;
    ci.sharingMode = desc.sharingMode;
    ci.initialLayout = desc.initialLayout;
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
        m_memory = new MemoryImpl{m_device.m_device,
                                  static_cast<size_t>(requirements.size),
                                  index.value()};
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

}  // namespace nickel::graphics