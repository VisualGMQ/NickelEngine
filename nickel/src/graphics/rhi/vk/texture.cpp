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
    Assert(desc.format != rhi::TextureFormat::Presentation,
           "Present format texture can only get from "
           "device.GetPresentationTexture()");

    createImage(desc, queueIndices);
    allocMem(adapter.phyDevice,
             desc.viewFormat ? desc.viewFormat.value() : desc.format,
             desc.size);

    VK_CALL_NO_VALUE(dev_.device.bindImageMemory(image, mem, 0));
}

TextureImpl::TextureImpl(DeviceImpl& dev, vk::Image image, vk::DeviceMemory mem,
                         const Texture::Descriptor& desc)
    : rhi::TextureImpl{desc}, dev_{dev}, image{image}, mem{mem} {
    layouts.emplace_back(vk::ImageLayout::eUndefined);
}

bool isDepthOrStencil(TextureFormat fmt) {
    return fmt == TextureFormat::DEPTH32_FLOAT_STENCIL8 ||
           fmt == TextureFormat::DEPTH24_PLUS_STENCIL8 ||
           fmt == TextureFormat::DEPTH24_PLUS ||
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
        .setExtent({desc.size.width, desc.size.height, 1})
        .setArrayLayers(desc.size.depthOrArrayLayers)
        .setUsage(TextureUsage2Vk(desc.usage, isDepthOrStencil(desc.format)));
    if (desc.flags & TextureFlagBits::CubeCompatible) {
        info.setFlags(vk::ImageCreateFlagBits::eCubeCompatible);
    }

    if (queueIndices.size() > 1) {
        info.setQueueFamilyIndices(queueIndices)
            .setSharingMode(vk::SharingMode::eConcurrent);
    } else {
        info.setSharingMode(vk::SharingMode::eExclusive);
    }

    layouts.resize(desc.size.depthOrArrayLayers, vk::ImageLayout::eUndefined);

    VK_CALL(image, dev_.device.createImage(info));
}

void TextureImpl::allocMem(vk::PhysicalDevice phyDevice, TextureFormat,
                           const Extent3D&) {
    auto requirements = dev_.device.getImageMemoryRequirements(image);
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
    if (image) {
        dev_.device.destroy(image);
    }
}

TextureView TextureImpl::CreateView(const TextureView::Descriptor& desc) {
    return TextureView(APIPreference::Vulkan, dev_, *this, desc);
}

vk::Image TextureImpl::GetImage() const {
    return image;
}

}  // namespace nickel::rhi::vulkan