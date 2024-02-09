#pragma once

#include "common/cgmath.hpp"
#include "graphics/rhi/texture.hpp"
#include "graphics/rhi/texture_view.hpp"
#include "graphics/rhi/vk/pch.hpp"


namespace nickel::rhi::vulkan {

class DeviceImpl;

class Swapchain final {
public:
    Swapchain() = default;

    void Init(vk::PhysicalDevice phyDev, DeviceImpl& dev,
              vk::SurfaceKHR surface, void* window);
    void Destroy(vk::Device);

    auto& ImageInfo() const { return imageInfo; }

    const std::vector<vk::Image>& Images() const { return images; }

    const std::vector<vk::ImageView>& ImageViews() const { return imageViews; }

    struct ImageInfo {
        vk::Extent2D extent;
        uint32_t imagCount;
        vk::SurfaceFormatKHR format;
    } imageInfo;

    vk::SwapchainKHR swapchain;
    std::vector<vk::Image> images;
    std::vector<vk::ImageView> imageViews;

private:
    struct ImageInfo queryImageInfo(vk::PhysicalDevice, const cgmath::Vec2&,
                                    vk::SurfaceKHR);
    vk::PresentModeKHR queryPresentMode(vk::PhysicalDevice, vk::SurfaceKHR);

    void getAndCreateImageViews(DeviceImpl&);
};

}  // namespace nickel::rhi::vulkan