#pragma once

#include "common/cgmath.hpp"
#include "vulkan/pch.hpp"
#include "stdpch.hpp"
#include "vulkan/sync.hpp"
#include "video/window.hpp"

namespace nickel::vulkan {

class Device;

class Swapchain final {
public:
    Swapchain(Device&, vk::SurfaceKHR, const Window& window);
    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    Swapchain(Swapchain&& o) { swap(o, *this); }

    Swapchain& operator=(Swapchain&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~Swapchain();

    auto Raw() const { return swapchain_; }

    auto& ImageInfo() const { return imageInfo_; }

    const std::vector<vk::Image>& Images() const { return images_; }

    const std::vector<vk::ImageView>& ImageViews() const { return imageViews_; }

    operator vk::SwapchainKHR() { return swapchain_; }

    operator vk::SwapchainKHR() const { return swapchain_; }

    uint32_t AcquireNextImage(std::optional<uint64_t> timeout, const Semaphore*, const Fence* fence);

private:
    struct ImageInfo {
        vk::Extent2D extent;
        uint32_t imagCount;
        vk::SurfaceFormatKHR format;
    } imageInfo_;

    vk::SwapchainKHR swapchain_;
    Device* device_{};
    std::vector<vk::Image> images_;
    std::vector<vk::ImageView> imageViews_;
    const Window* window_{};

    struct ImageInfo queryImageInfo(const Window& window, vk::SurfaceKHR);
    vk::PresentModeKHR queryPresentMode(vk::SurfaceKHR);

    void getAndCreateImageViews();

    friend void swap(Swapchain& o1, Swapchain& o2) noexcept {
        using std::swap;

        swap(o1.swapchain_, o2.swapchain_);
        swap(o1.device_, o2.device_);
        swap(o1.images_, o2.images_);
        swap(o1.imageViews_, o2.imageViews_);
        swap(o1.window_, o2.window_);
    }
};

}  // namespace nickel::vulkan