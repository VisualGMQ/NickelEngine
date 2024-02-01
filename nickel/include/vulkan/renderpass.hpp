#pragma once

#include "vulkan/pch.hpp"
#include "stdpch.hpp"

namespace nickel::vulkan {

class Device;

class RenderPass final {
public:
    RenderPass(Device* device,
               const std::vector<vk::AttachmentDescription>& attachmentDescs,
               const std::vector<vk::SubpassDescription>& subpasses,
               const std::vector<vk::SubpassDependency>& subpassDeps);
    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

    RenderPass(RenderPass&& o) { swap(o, *this); }

    RenderPass& operator=(RenderPass&& o) {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    ~RenderPass();

    operator vk::RenderPass() const { return renderPass_; }
    operator vk::RenderPass() { return renderPass_; }

private:
    Device* device_{};
    vk::RenderPass renderPass_;

    friend void swap(RenderPass& o1, RenderPass& o2) noexcept {
        using std::swap;

        swap(o1.device_, o2.device_);
        swap(o1.renderPass_, o2.renderPass_);
    }
};

}  // namespace nickel::vulkan