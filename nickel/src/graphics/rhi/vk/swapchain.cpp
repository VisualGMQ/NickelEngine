#include "graphics/rhi/vk/swapchain.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/util.hpp"
#include "graphics/rhi/vk/texture.hpp"
#include "graphics/rhi/vk/texture_view.hpp"

namespace nickel::rhi::vulkan {

Swapchain::Swapchain(vk::PhysicalDevice phyDev, DeviceImpl& dev,
                     vk::SurfaceKHR surface, const cgmath::Vec2& windowSize) {
    vk::SurfaceCapabilitiesKHR capacities;
    VK_CALL(capacities, phyDev.getSurfaceCapabilitiesKHR(surface));

    imageInfo = queryImageInfo(phyDev, windowSize, surface);
    auto [extent, imageCount, format] = imageInfo;
    auto presentMode = queryPresentMode(phyDev, surface);

    auto& queueIndices = dev.queueIndices;

    std::set<uint32_t> uniqueIndices{queueIndices.graphicsIndex.value(),
                                     queueIndices.presentIndex.value()};
                                     
    std::vector<uint32_t> indices;
    for (auto idx : uniqueIndices) {
        indices.push_back(idx);
    }

    vk::SwapchainCreateInfoKHR createInfo;
    createInfo.setSurface(surface)
        .setClipped(true)
        .setPresentMode(presentMode)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment |
                       vk::ImageUsageFlagBits::eTransferDst)
        .setImageExtent(extent)
        .setMinImageCount(imageCount)
        .setImageFormat(format.format)
        .setImageColorSpace(format.colorSpace)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setImageArrayLayers(1)
        .setPreTransform(capacities.currentTransform)
        .setImageSharingMode(queueIndices.graphicsIndex.value() ==
                                     queueIndices.presentIndex.value()
                                 ? vk::SharingMode::eExclusive
                                 : vk::SharingMode::eConcurrent)
        .setQueueFamilyIndices(indices);
    VK_CALL(swapchain, dev.device.createSwapchainKHR(createInfo));

    if (swapchain) {
        getAndCreateImageViews(dev);
    }
}

struct Swapchain::ImageInfo Swapchain::queryImageInfo(
    vk::PhysicalDevice dev, const cgmath::Vec2& winSize,
    vk::SurfaceKHR surface) {
    vk::SurfaceCapabilitiesKHR capacities;
    VK_CALL(capacities, dev.getSurfaceCapabilitiesKHR(surface));

    vk::Extent2D extent(winSize.x, winSize.y);
    extent.width = std::clamp(extent.width, capacities.minImageExtent.width,
                              capacities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capacities.minImageExtent.height,
                               capacities.maxImageExtent.height);

    auto imageCount = std::clamp<uint32_t>(2, capacities.minImageCount,
                                           capacities.maxImageCount);

    std::vector<vk::SurfaceFormatKHR> formats;
    VK_CALL(formats, dev.getSurfaceFormatsKHR(surface));
    vk::SurfaceFormatKHR* chooseFormat = &formats[0];
    for (auto& format : formats) {
        if (format.format == vk::Format::eR8G8B8A8Unorm &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            chooseFormat = &format;
            break;
        }
    }

    return {extent, imageCount, *chooseFormat};
}

vk::PresentModeKHR Swapchain::queryPresentMode(vk::PhysicalDevice dev,
                                               vk::SurfaceKHR surface) {
    std::vector<vk::PresentModeKHR> presentModes;
    VK_CALL(presentModes, dev.getSurfacePresentModesKHR(surface));
    vk::PresentModeKHR choose = vk::PresentModeKHR::eFifo;
    for (auto mode : presentModes) {
        if (mode == vk::PresentModeKHR::eMailbox) {
            choose = mode;
            break;
        }
    }

    return choose;
}

void Swapchain::Destroy(vk::Device dev) {
    if (swapchain) {
        for (auto& view : imageViews) {
            delete view;
        }
        for (auto& image : images) {
            image->image = nullptr;
            delete image;
        }
        dev.destroySwapchainKHR(swapchain);
    }
}

void Swapchain::getAndCreateImageViews(DeviceImpl& dev) {
    std::vector<vk::Image> images;
    VK_CALL(images, dev.device.getSwapchainImagesKHR(swapchain));

    Texture::Descriptor desc;
    desc.size.width = ImageInfo().extent.width;
    desc.size.height = ImageInfo().extent.height;
    desc.size.depthOrArrayLayers = 1;
    desc.usage = TextureUsage::RenderAttachment;
    desc.sampleCount = SampleCount::Count1;
    desc.mipmapLevelCount = 1;
    desc.dimension = TextureType::Dim2;
    desc.format = TextureFormat::Presentation;

    for (auto& image : images) {
        auto& texture =
            this->images.emplace_back(new TextureImpl{dev, image, {}, desc});

        vk::ComponentMapping mapping;
        vk::ImageSubresourceRange range;
        range.setLevelCount(1)
            .setLayerCount(1)
            .setBaseArrayLayer(0)
            .setBaseMipLevel(0)
            .setAspectMask(vk::ImageAspectFlagBits::eColor);
        vk::ImageViewCreateInfo createInfo;
        createInfo.setImage(image)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(imageInfo.format.format)
            .setComponents(mapping)
            .setSubresourceRange(range);

        vk::ImageView view;
        VK_CALL(view, dev.device.createImageView(createInfo));
        if (view) {
            imageViews.emplace_back(new TextureViewImpl{dev, *texture, view});
        }
    }
}

}  // namespace nickel::rhi::vulkan