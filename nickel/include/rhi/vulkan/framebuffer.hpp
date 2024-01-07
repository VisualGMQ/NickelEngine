#pragma once

#include "pch.hpp"
#include "rhi/framebuffer.hpp"
#include "rhi/vulkan/image.hpp"
#include "rhi/vulkan/renderpass.hpp"


namespace nickel::rhi::vulkan {

class Device;

class Framebuffer : public rhi::Framebuffer {
public:
    Framebuffer(Device* device, uint32_t w, uint32_t h,
                const std::vector<ImageView>& imageViews, uint32_t layers,
                const RenderPass& renderPass);
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& o) { swap(o, *this); }

    Framebuffer& operator=(Framebuffer&& o) {
        if (this != &o) {
            swap(o, *this);
        }
        return *this;
    }

    ~Framebuffer();

    auto& Raw() const { return fbo_; }

private:
    Device* device_{};
    vk::Framebuffer fbo_;

    friend void swap(Framebuffer& o1, Framebuffer& o2) {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.fbo_, o2.fbo_);
    }
};

}  // namespace nickel::rhi::vulkan