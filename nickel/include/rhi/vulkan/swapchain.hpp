#pragma once

#include "pch.hpp"
#include "rhi/vulkan/image.hpp"
#include "rhi/vulkan/surface.hpp"


namespace nickel::rhi::vulkan {

class Device;

class Swapchain {
public:
    struct ImageInfo {
        vk::Extent2D extent;
        uint32_t imagCount;
        vk::SurfaceFormatKHR format;
    } imageInfo_;

    Swapchain(Device* device, Surface&& surface);
    auto Raw() const { return swapchain_; }
    auto& ImageInfo() const { return imageInfo_; }
    ~Swapchain();

private:
    vk::SwapchainKHR swapchain_;
    Surface surface_;
    Device* device_;
    std::vector<vk::Image> images_;
    std::vector<vk::ImageView> imageViews_;

    struct ImageInfo queryImageInfo();
    vk::PresentModeKHR queryPresentMode();

    void getAndCreateImageViews();
};

}  // namespace nickel::rhi::vulkan
