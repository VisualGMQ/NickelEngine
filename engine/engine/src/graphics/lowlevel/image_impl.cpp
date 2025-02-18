#include "nickel/graphics/lowlevel/internal/image_impl.hpp"

#include "nickel/graphics/lowlevel/internal/adapter_impl.hpp"
#include "nickel/graphics/lowlevel/internal/common.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/memory_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

ImageImpl::ImageImpl(const AdapterImpl& adapter, DeviceImpl& dev,
                     const Image::Descriptor& desc)
    : m_device{dev} {
    createImage(desc, dev);
    allocMem(adapter.m_phy_device);

    for (int i = 0; i < desc.m_array_layers; i++) {
        m_layouts.push_back(desc.m_initial_layout);
    }

    VK_CALL(
        vkBindImageMemory(m_device.m_device, m_image, m_memory->m_memory, 0));
}

void ImageImpl::createImage(const Image::Descriptor& desc, DeviceImpl& dev) {
    VkFormatFeatureFlags features = 0;
    auto usage = desc.m_usage;
    if (usage & ImageUsage::Sampled) {
        features |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    }
    if (usage & ImageUsage::ColorAttachment) {
        features |= VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
    }
    if (usage & ImageUsage::CopyDst) {
        features |= VK_FORMAT_FEATURE_TRANSFER_DST_BIT;
    }
    if (usage & ImageUsage::CopySrc) {
        features |= VK_FORMAT_FEATURE_TRANSFER_SRC_BIT;
    }
    if (usage & ImageUsage::StorageBinding) {
        features |= VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT;
    } 
    if (usage & ImageUsage::DepthStencilAttachment) {
        features |= VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }

    // TODO: if format not support, return a similar format and report a warning
    findSupportedFormat(Format2Vk(desc.m_format), ImageTiling2Vk(desc.m_tiling), features);
    
    VkImageCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    if (desc.m_is_cube_map) {
        ci.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }
    ci.imageType = ImageType2Vk(desc.m_image_type);
    ci.format = Format2Vk(desc.m_format);
    ci.extent = VkExtent3D{desc.m_extent.w, desc.m_extent.h, desc.m_extent.l};
    ci.mipLevels = desc.m_mip_levels;
    ci.arrayLayers = desc.m_array_layers;
    ci.samples =
        static_cast<VkSampleCountFlagBits>(SampleCount2Vk(desc.m_samples));
    ci.tiling = ImageTiling2Vk(desc.m_tiling);
    ci.usage = ImageUsage2Vk(desc.m_usage);
    ci.sharingMode = SharingMode2Vk(desc.m_sharing_mode);
    ci.initialLayout = ImageLayout2Vk(desc.m_initial_layout);
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

void ImageImpl::findSupportedFormat(VkFormat candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(m_device.GetAdapter().m_phy_device, candidates, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR &&
        (props.linearTilingFeatures & features) == features) {
        return;
    }
    if (tiling == VK_IMAGE_TILING_OPTIMAL &&
        (props.optimalTilingFeatures & features) == features) {
        return;
    }

    LOGC("failed to find supported format!");
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
            new MemoryImpl{m_device, static_cast<uint64_t>(requirements.size), index.value()};
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

void ImageImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_device.m_image_allocator.MarkAsGarbage(this);
    }
}

}  // namespace nickel::graphics