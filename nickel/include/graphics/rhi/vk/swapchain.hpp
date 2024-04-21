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
    Swapchain(vk::PhysicalDevice phyDev, DeviceImpl& dev,
              vk::SurfaceKHR surface, const cgmath::Vec2& windowSize);

    Swapchain(Swapchain&& o) noexcept { swap(o, *this); }

    Swapchain& operator=(Swapchain&& o) noexcept {
        if (&o != this) swap(o, *this);
        return *this;
    }

    Swapchain(const Swapchain& o) = delete;
    Swapchain& operator=(const Swapchain& o) = delete;

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

    friend void swap(Swapchain& o1, Swapchain& o2) noexcept {
        using std::swap;

        swap(o1.imageInfo, o2.imageInfo);
        swap(o1.swapchain, o2.swapchain);
        swap(o1.images, o2.images);
        swap(o1.imageViews, o2.imageViews);
    }
};

}  // namespace nickel::rhi::vulkan