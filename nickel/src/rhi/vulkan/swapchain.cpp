#include "rhi/vulkan/swapchain.hpp"
#include "rhi/vulkan/device.hpp"
#include "rhi/vulkan/instance.hpp"
#include "window/window.hpp"

namespace nickel::rhi::vulkan {

Swapchain::Swapchain(Device* device, Surface&& surface)
    : device_(device), surface_(std::move(surface)) {
    auto& dev = device_->Raw();

    auto capacities =
        device->GetPhysicalDevice().getSurfaceCapabilitiesKHR(surface_.Raw());

    imageInfo_ = queryImageInfo();
    auto [extent, imageCount, format] = imageInfo_;
    auto presentMode = queryPresentMode();

    auto& queueIndices = device_->GetQueueIndices();

    std::set<uint32_t> uniqueIndices{queueIndices.graphicsIndex.value(),
                                     queueIndices.presentIndex.value()};

    std::vector<uint32_t> indices;
    for (auto idx : uniqueIndices) {
        indices.push_back(idx);
    }

    vk::SwapchainCreateInfoKHR createInfo;
    createInfo.setSurface(surface_.Raw())
        .setClipped(true)
        .setPresentMode(presentMode)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
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
    swapchain_ = dev.createSwapchainKHR(createInfo);

    if (!swapchain_) {
        LOGE(nickel::log_tag::Vulkan, "create swpachain failed");
    } else {
        getAndCreateImageViews();
    }
}

struct Swapchain::ImageInfo Swapchain::queryImageInfo() {
    auto capacities =
        device_->GetPhysicalDevice().getSurfaceCapabilitiesKHR(surface_.Raw());

    auto winSize = device_->GetInstance().GetWindow().Size();
    vk::Extent2D extent(winSize.x, winSize.y);
    extent.width = std::clamp(extent.width, capacities.minImageExtent.width,
                              capacities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capacities.minImageExtent.height,
                              capacities.maxImageExtent.height);

    auto imageCount = std::clamp<uint32_t>(2, capacities.minImageCount,
                                               capacities.maxImageCount);

    auto formats =
        device_->GetPhysicalDevice().getSurfaceFormatsKHR(surface_.Raw());
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
    auto presentModes = device_->GetPhysicalDevice().getSurfacePresentModesKHR(surface_.Raw());
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
        for (auto& img : images_) {
            device_->Raw().destroyImage(img);
        }
        for (auto& view : imageViews_) {
            device_->Raw().destroyImageView(view);
        }
        device_->Raw().destroySwapchainKHR(swapchain_);
    }
}

void Swapchain::getAndCreateImageViews() {
        auto images = device_->Raw().getSwapchainImagesKHR(swapchain_);
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

            vk::ImageView view = device_->Raw().createImageView(createInfo);
            if (!view) {
                LOGE(nickel::log_tag::Vulkan, "create image view from swapchain failed!");
            } else {
                imageViews_.emplace_back(view);
            }
        }
    }

}  // namespace nickel::rhi::vulkan