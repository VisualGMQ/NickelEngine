#include "vulkan/image.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"

namespace nickel::vulkan {

Image::Image(Device *device, vk::ImageType type, vk::ImageViewType viewType,
             const vk::Extent3D &extent, vk::Format format,
             vk::Format viewFormat, vk::ImageLayout initLayout,
             uint32_t arrayLayer, uint32_t mipLevel,
             vk::SampleCountFlagBits sampleCount, vk::ImageUsageFlagBits usage,
             vk::ImageTiling tiling, const vk::ComponentMapping &components,
             const vk::ImageSubresourceRange &subresourceRange,
             std::vector<uint32_t> queueIndices)
    : device_{device} {
    vk::ImageCreateInfo imgCreateInfo;
    imgCreateInfo.setQueueFamilyIndices(queueIndices)
        .setImageType(type)
        .setExtent(extent)
        .setFormat(format)
        .setInitialLayout(initLayout)
        .setArrayLayers(arrayLayer)
        .setMipLevels(mipLevel)
        .setSamples(sampleCount)
        .setUsage(usage)
        .setTiling(tiling);
    VK_CALL(image_, device->GetDevice().createImage(imgCreateInfo));

    ChangeView(viewType, viewFormat, components, subresourceRange);
}

void Image::ChangeView(vk::ImageViewType type, vk::Format format,
                       const vk::ComponentMapping &componentMapping,
                       const vk::ImageSubresourceRange &range) {
    vk::ImageViewCreateInfo viewCreateInfo;
    viewCreateInfo.setImage(image_)
        .setViewType(type)
        .setFormat(format)
        .setComponents(componentMapping)
        .setSubresourceRange(range);
    VK_CALL(view_, device_->GetDevice().createImageView(viewCreateInfo));
}

Image::~Image() {
    if (device_) {
        if (view_) {
            device_->GetDevice().destroyImageView(view_);
        }
        if (image_) {
            device_->GetDevice().destroyImage(image_);
        }
    }
}

Framebuffer::Framebuffer(Device *device,
                         const std::vector<vk::ImageView> &views,
                         uint32_t width, uint32_t height, uint32_t layers,
                         vk::RenderPass renderPass)
    : device_{device} {
    vk::FramebufferCreateInfo info;
    info.setAttachments(views)
        .setWidth(width)
        .setHeight(height)
        .setRenderPass(renderPass)
        .setLayers(layers);

    VK_CALL(fbo_, device_->GetDevice().createFramebuffer(info));
}

Framebuffer::~Framebuffer() {
    if (device_) {
        device_->GetDevice().destroyFramebuffer(fbo_);
    }
}

Sampler::Sampler(Device *device, vk::Filter min, vk::Filter mag,
                 vk::SamplerMipmapMode mipmap, vk::SamplerAddressMode u,
                 vk::SamplerAddressMode v, vk::SamplerAddressMode w,
                 float mipLodBias, bool anisotropyEnable, float maxAnisotropy,
                 bool compareEnable, vk::CompareOp compare, float minLod,
                 float maxLod, vk::BorderColor borderColor,
                 bool unnormalizedCoordinates)
    : device_{device} {
    vk::SamplerCreateInfo info;
    info.setMinFilter(min)
        .setMagFilter(mag)
        .setMipmapMode(mipmap)
        .setAddressModeU(u)
        .setAddressModeV(v)
        .setAddressModeW(w)
        .setMipLodBias(mipLodBias)
        .setAnisotropyEnable(anisotropyEnable)
        .setMaxAnisotropy(maxAnisotropy)
        .setCompareEnable(compareEnable)
        .setCompareOp(compare)
        .setMinLod(minLod)
        .setMaxLod(maxLod)
        .setBorderColor(borderColor)
        .setUnnormalizedCoordinates(unnormalizedCoordinates);
    VK_CALL(sampler_, device->GetDevice().createSampler(info));
}

Sampler::~Sampler() {
    if (device_) {
        device_->GetDevice().destroySampler(sampler_);
    }
}

}  // namespace nickel::vulkan