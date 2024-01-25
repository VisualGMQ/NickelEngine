#include "vulkan/swapchain.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"

namespace nickel::vulkan {

Swapchain::Swapchain(Device& dev, vk::SurfaceKHR surface, const Window& window)
    : device_(dev), surface_(surface), window_{window} {
    vk::SurfaceCapabilitiesKHR capacities;
    VK_CALL(capacities,
            device_.GetPhyDevice().getSurfaceCapabilitiesKHR(surface_));

    imageInfo_ = queryImageInfo(window);
    auto [extent, imageCount, format] = imageInfo_;
    auto presentMode = queryPresentMode();

    auto& queueIndices = device_.GetQueueFamilyIndices();

    std::set<uint32_t> uniqueIndices{queueIndices.graphicsIndex.value(),
                                     queueIndices.presentIndex.value()};

    std::vector<uint32_t> indices;
    for (auto idx : uniqueIndices) {
        indices.push_back(idx);
    }

    vk::SwapchainCreateInfoKHR createInfo;
    createInfo.setSurface(surface_)
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
    VK_CALL(swapchain_, device_.GetDevice().createSwapchainKHR(createInfo));

    if (swapchain_) {
        getAndCreateImageViews();
    }
}

struct Swapchain::ImageInfo Swapchain::queryImageInfo(const Window& window) {
    vk::SurfaceCapabilitiesKHR capacities;
    VK_CALL(capacities,
            device_.GetPhyDevice().getSurfaceCapabilitiesKHR(surface_));

    auto winSize = window.Size();
    vk::Extent2D extent(winSize.x, winSize.y);
    extent.width = std::clamp(extent.width, capacities.minImageExtent.width,
                              capacities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capacities.minImageExtent.height,
                               capacities.maxImageExtent.height);

    auto imageCount = std::clamp<uint32_t>(2, capacities.minImageCount,
                                           capacities.maxImageCount);

    std::vector<vk::SurfaceFormatKHR> formats;
    VK_CALL(formats, device_.GetPhyDevice().getSurfaceFormatsKHR(surface_));
    vk::SurfaceFormatKHR* chooseFormat = &formats[0];
    for (auto& format : formats) {
        if (format.format == vk::Format::eR8G8B8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            chooseFormat = &format;
            break;
        }
    }

    return {extent, imageCount, *chooseFormat};
}

vk::PresentModeKHR Swapchain::queryPresentMode() {
    std::vector<vk::PresentModeKHR> presentModes;
    VK_CALL(presentModes,
            device_.GetPhyDevice().getSurfacePresentModesKHR(surface_));
    vk::PresentModeKHR choose = vk::PresentModeKHR::eFifo;
    for (auto mode : presentModes) {
        if (mode == vk::PresentModeKHR::eMailbox) {
            choose = mode;
            break;
        }
    }

    return choose;
}

Swapchain::~Swapchain() {
    if (swapchain_) {
        for (auto& view : imageViews_) {
            device_.GetDevice().destroyImageView(view);
        }
        for (auto& img : images_) {
            device_.GetDevice().destroyImage(img);
        }
        device_.GetDevice().destroySwapchainKHR(swapchain_);
    }
}

void Swapchain::getAndCreateImageViews() {
    std::vector<vk::Image> images;
    VK_CALL(images, device_.GetDevice().getSwapchainImagesKHR(swapchain_));
    for (auto& image : images) {
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
            .setFormat(imageInfo_.format.format)
            .setComponents(mapping)
            .setSubresourceRange(range);

        vk::ImageView view;
        VK_CALL(view, device_.GetDevice().createImageView(createInfo));
        if (view) {
            imageViews_.emplace_back(view);
        }
        images_.emplace_back(image);
    }
}

}  // namespace nickel::vulkan