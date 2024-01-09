#include "rhi/vulkan/image.hpp"
#include "rhi/vulkan/device.hpp"
#include "rhi/vulkan/enum_convert.hpp"

namespace nickel::rhi::vulkan {

inline vk::ComponentMapping ComponentMapping2Vk(
    const ComponentMapping& mapping) {
    vk::ComponentMapping m;
    m.setA(ComponentSwizzle2Vk(mapping.a))
        .setR(ComponentSwizzle2Vk(mapping.r))
        .setG(ComponentSwizzle2Vk(mapping.g))
        .setB(ComponentSwizzle2Vk(mapping.b));
    return m;
}

inline vk::ImageSubresourceRange ImageSubresourceRange2Vk(
    const ImageSubresourceRange& range) {
    vk::ImageSubresourceRange r;
    r.setAspectMask(ImageAspectFlags2Vk(range.aspectMask))
        .setBaseArrayLayer(range.baseArrayLayer)
        .setBaseMipLevel(range.baseMipmapLevel)
        .setLevelCount(range.levelCount)
        .setLayerCount(range.layerCount);
    return r;
}

ImageView::ImageView(Device* device, const Image& image, ImageViewType viewType,
                     enum Format format, const ComponentMapping& mapping,
                     const ImageSubresourceRange& subresourceRange)
    : rhi::ImageView{viewType, format}, device_{device} {
    vk::ImageViewCreateInfo createInfo;
    vk::ImageSubresourceRange range;
    vk::ImageAspectFlagBits flags;
    createInfo.setImage(image.Raw())
        .setViewType(ImageViewType2Vk(viewType))
        .setFormat(Format2Vk(format))
        .setComponents(ComponentMapping2Vk(mapping))
        .setSubresourceRange(ImageSubresourceRange2Vk(subresourceRange));

    view_ = device->Raw().createImageView(createInfo);
    if (!view_) {
        LOGE(log_tag::Vulkan, "create image view failed");
    }
}

ImageView::~ImageView() {
    if (device_ && view_) {
        device_->Raw().destroyImageView(view_);
    }
}

Image::Image(
    Device* device, ImageType type, const cgmath::Vec3& extent,
    enum Format format, ImageLayout initLayout, uint32_t arrayLayer,
    uint32_t mipLevel, SampleCountFlag sampleCount, ImageUsageFlags usage,
    ImageTiling tiling,
    std::optional<std::reference_wrapper<std::vector<uint32_t>>> queueIndices)
    : rhi::Image{type, extent, format, usage}, device_(device) {
    vk::ImageCreateInfo createInfo;
    createInfo.setImageType(ImageType2Vk(type))
        .setArrayLayers(arrayLayer)
        .setMipLevels(mipLevel)
        .setInitialLayout(ImageLayout2Vk(initLayout))
        .setExtent(vk::Extent3D(extent.x, extent.y, extent.z))
        .setFormat(Format2Vk(format))
        .setSamples(SampleCountFlag2Vk(sampleCount))
        .setUsage(ImageUsageFlags2Vk(usage))
        .setTiling(ImageTiling2Vk(tiling));

    if (queueIndices) {
        createInfo.setQueueFamilyIndices(queueIndices.value().get());
        createInfo.setSharingMode(vk::SharingMode::eConcurrent);
    } else {
        createInfo.setSharingMode(vk::SharingMode::eExclusive);
    }

    image_ = device->Raw().createImage(createInfo);
    if (!image_) {
        LOGE(log_tag::Vulkan, "create image failed");
    }
}

Image::~Image() {
    if (device_ && image_) {
        device_->Raw().destroyImage(image_);
    }
}

Sampler::Sampler(Device* device, Filter min, Filter mag, SamplerAddressMode u,
                 SamplerAddressMode v, SamplerAddressMode w, float mipLodBias,
                 bool anisotropyEnable, float maxAnisotropy, bool compareEnable,
                 CompareOp compareOp, float minLod, float maxLod,
                 BorderColor borderColor, bool unormalizedCoordinates): device_{device} {
    vk::SamplerCreateInfo createInfo;
    createInfo.setMinFilter(Filter2Vk(min))
        .setMagFilter(Filter2Vk(mag))
        .setAddressModeU(SamplerAddressMode2Vk(u))
        .setAddressModeV(SamplerAddressMode2Vk(v))
        .setAddressModeW(SamplerAddressMode2Vk(w))
        .setMipLodBias(mipLodBias)
        .setAnisotropyEnable(anisotropyEnable)
        .setMaxAnisotropy(maxAnisotropy)
        .setCompareEnable(compareEnable)
        .setCompareOp(CompareOp2Vk(compareOp))
        .setMinLod(minLod)
        .setMaxLod(maxLod)
        .setBorderColor(BorderColor2Vk(borderColor))
        .setUnnormalizedCoordinates(unormalizedCoordinates);

    sampler_ = device_->Raw().createSampler(createInfo);
    if (!sampler_) {
        LOGE(log_tag::Vulkan, "create sampler failed");
    }
}

Sampler::~Sampler() {
    if (device_ && sampler_) {
        device_->Raw().destroySampler(sampler_);
    }
}

}  // namespace nickel::rhi::vulkan