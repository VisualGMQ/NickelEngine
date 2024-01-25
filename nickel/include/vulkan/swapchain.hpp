#pragma once

#include "core/cgmath.hpp"
#include "pch.hpp"
#include "window/window.hpp"

namespace nickel::vulkan {

class Device;

class Swapchain final {
public:
    Swapchain(Device&, vk::SurfaceKHR, const Window& window);

    ~Swapchain();

    auto Raw() const { return swapchain_; }

    auto& ImageInfo() const { return imageInfo_; }

    std::vector<vk::Image> Images() const { return images_; }

    std::vector<vk::ImageView> ImageViews() const { return imageViews_; }

    cgmath::Vec<uint32_t, 2> ImageExtent() const {
        return {imageInfo_.extent.width, imageInfo_.extent.height};
    }

    operator vk::SwapchainKHR() { return swapchain_; }

    operator vk::SwapchainKHR() const { return swapchain_; }

private:
    struct ImageInfo {
        vk::Extent2D extent;
        uint32_t imagCount;
        vk::SurfaceFormatKHR format;
    } imageInfo_;

    vk::SwapchainKHR swapchain_;
    vk::SurfaceKHR surface_;
    Device& device_;
    std::vector<vk::Image> images_;
    std::vector<vk::ImageView> imageViews_;
    const Window& window_;

    struct ImageInfo queryImageInfo(const Window& window);
    vk::PresentModeKHR queryPresentMode();

    void getAndCreateImageViews();
};

}  // namespace nickel::vulkan