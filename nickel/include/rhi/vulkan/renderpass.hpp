#pragma once

#include "pch.hpp"
#include "rhi/renderpass.hpp"

namespace nickel::rhi::vulkan {

class Device;

class RenderPass : public rhi::RenderPass {
public:
    RenderPass(Device* device,
               const std::vector<AttachmentDescription>& attachmentDescs,
               const std::vector<SubpassDescription>& subpasses,
               const std::vector<SubpassDependency>& subpassDeps);
    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

    RenderPass(RenderPass&& o) { swap(*this, o); }

    RenderPass& operator=(RenderPass&& o) {
        if (this != &o) {
            swap(*this, o);
        }
        return *this;
    }

    auto& Raw() const { return renderPass_; }

    ~RenderPass();

private:
    Device* device_;
    vk::RenderPass renderPass_;

    friend void swap(RenderPass& o1, RenderPass& o2) {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.renderPass_, o2.renderPass_);
    }
};

}  // namespace nickel::rhi::vulkan