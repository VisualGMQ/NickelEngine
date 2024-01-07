#pragma once

#include "pch.hpp"
#include "rhi/device.hpp"
#include "rhi/vulkan/instance.hpp"
#include "window/window.hpp"


namespace nickel::rhi::vulkan {

class Surface {
public:
    explicit Surface(Instance& instance) : instance_(&instance) {
        VkSurfaceKHR surface;
        SDL_Vulkan_CreateSurface(
            (SDL_Window*)instance.GetWindow().Raw(),
            instance.Raw(), &surface);

        if (!surface) {
            LOGW(nickel::log_tag::Vulkan, "create surface failed");
        } else {
            surface_ = surface;
        }
    }

    Surface(const Surface&) = delete;
    Surface(Surface&& o) {
        swap(o, *this);
    }

    Surface& operator=(const Surface&) = delete;
    Surface& operator=(Surface&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~Surface() {
        if (surface_) {
            instance_->Raw().destroySurfaceKHR(surface_);
        }
    }

    auto Raw() const { return surface_; }

    auto Raw() { return surface_; }

private:
    vk::SurfaceKHR surface_;
    Instance* instance_ = nullptr;

    friend void swap(Surface& o1, Surface& o2) noexcept {
        using std::swap;

        swap(o1.instance_, o2.instance_);
        swap(o1.surface_, o2.surface_);
    }
};

}  // namespace nickel::rhi::vulkan