#include "graphics/rhi/vk/texture.hpp"
#include "graphics/rhi/vk/adapter.hpp"
#include "graphics/rhi/vk/convert.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/memory.hpp"


namespace nickel::rhi::vulkan {

TextureImpl::TextureImpl(AdapterImpl& adapter, DeviceImpl& dev,
                         const Texture::Descriptor& desc,
                         const std::vector<uint32_t>& queueIndices)
    : rhi::TextureImpl{desc}, dev_{dev} {
    if (desc.format != TextureFormat::Presentation) {
        createImage(desc, queueIndices);
        allocMem(adapter.phyDevice,
                desc.viewFormat ? desc.viewFormat.value() : desc.format,
                desc.size);
        VK_CALL_NO_VALUE(dev_.device.bindImageMemory(image_, mem, 0));
    } else {
        desc_.size.width = dev.swapchain.ImageInfo().extent.width;
        desc_.size.height = dev.swapchain.ImageInfo().extent.height;
        desc_.size.depthOrArrayLayers = 1;
        desc_.usage = TextureUsage::RenderAttachment;
        desc_.sampleCount = SampleCount::Count1;
        desc_.mipmapLevelCount = 1;
        desc_.dimension = TextureType::Dim2;
    }
}

bool isDepthStencil(TextureFormat fmt) {
    return fmt == TextureFormat::DEPTH24_PLUS_STENCIL8 ||
           fmt == TextureFormat::DEPTH24_PLUS_STENCIL8 ||
           fmt == TextureFormat::DEPTH24_PLUS_STENCIL8 ||
           fmt == TextureFormat::DEPTH16_UNORM ||
           fmt == TextureFormat::DEPTH32_FLOAT;
}

void TextureImpl::createImage(const Texture::Descriptor& desc,
                              const std::vector<uint32_t>& queueIndices) {
    vk::ImageCreateInfo info;
    info.setImageType(TextureType2Vk(desc.dimension))
        .setFormat(TextureFormat2Vk(desc.format))
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setMipLevels(desc.mipmapLevelCount)
        .setSamples(SampleCount2Vk(desc.sampleCount))
        .setExtent(
            {desc.size.width, desc.size.height, desc.size.depthOrArrayLayers})
        .setArrayLayers(desc.size.depthOrArrayLayers)
        .setUsage(TextureUsage2Vk(desc.usage, isDepthStencil(desc.format)));

    if (queueIndices.size() > 1) {
        info.setQueueFamilyIndices(queueIndices)
            .setSharingMode(vk::SharingMode::eConcurrent);
    } else {
        info.setSharingMode(vk::SharingMode::eExclusive);
    }

    VK_CALL(image_, dev_.device.createImage(info));
}

void TextureImpl::allocMem(vk::PhysicalDevice phyDevice, TextureFormat,
                           const Extent3D&) {
    auto requirements = dev_.device.getImageMemoryRequirements(image_);
    auto index = FindMemoryType(phyDevice, requirements,

                                vk::MemoryPropertyFlagBits::eDeviceLocal);
    if (!index) {
        LOGE(log_tag::Vulkan, "allocate image memory failed: no satisfied "
                              "memory type (DeviceLocal)");
    }

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.setAllocationSize(requirements.size)
        .setMemoryTypeIndex(index.value());
    VK_CALL(mem, dev_.device.allocateMemory(allocInfo));
}

TextureImpl::~TextureImpl() {
    if (mem) {
        dev_.device.freeMemory(mem);
    }
    if (image_) {
        dev_.device.destroy(image_);
    }
}

TextureView TextureImpl::CreateView(const TextureView::Descriptor& desc) {
    return TextureView(APIPreference::Vulkan, dev_, *this, desc);
}

vk::Image TextureImpl::GetImage() const {
    if (Descriptor().format == TextureFormat::Presentation) {
        return dev_.swapchain.Images()[dev_.curImageIndex];
    } else {
        return image_;
    }
}

}  // namespace nickel::rhi::vulkan