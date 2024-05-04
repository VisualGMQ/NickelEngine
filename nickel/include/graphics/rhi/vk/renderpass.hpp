#pragma once

#include "graphics/rhi/command.hpp"
#include "graphics/rhi/impl/renderpass.hpp"
#include "graphics/rhi/renderpass.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/util.hpp"

namespace nickel::rhi::vulkan {

inline vk::ImageLayout GetDepthStencilLayoutAfterRenderPass(
    const RenderPass::Descriptor& desc) {
    return GetLayoutByFormat(desc.depthStencilAttachment->view.Format(),
                             desc.depthStencilAttachment->stencilReadOnly,
                             desc.depthStencilAttachment->depthReadOnly);
}

inline vk::ImageLayout GetImageLayoutAfterRenderPass(TextureFormat viewFmt) {
    return GetLayoutByFormat(viewFmt);
}

class DeviceImpl;

struct SubpassInfo final {
    std::vector<vk::AttachmentReference> colorRefs;
    std::optional<vk::AttachmentReference> depthRef;
    bool isSelfDependency = false;

    bool operator==(const SubpassInfo& o) const noexcept {
        return std::equal(o.colorRefs.begin(), o.colorRefs.end(),
                          colorRefs.begin(), colorRefs.end()) &&
               depthRef == o.depthRef && isSelfDependency == o.isSelfDependency;
    }

    bool operator!=(const SubpassInfo& o) const noexcept {
        return !(*this == o);
    }
};

struct RenderPassInfo final {
    std::vector<vk::AttachmentDescription> descriptions;
    std::vector<SubpassInfo> subpasses;
    std::vector<vk::SubpassDependency> dependencies;

    bool operator==(const RenderPassInfo& o) const noexcept {
        return std::equal(descriptions.begin(), descriptions.end(),
                          o.descriptions.begin(), o.descriptions.end()) &&
               std::equal(subpasses.begin(), subpasses.end(),
                          o.subpasses.begin(), o.subpasses.end()) &&
               std::equal(dependencies.begin(), dependencies.end(),
                          o.dependencies.begin(), o.dependencies.end());
    }
};

class RenderPassImpl : public rhi::RenderPassImpl {
public:
    RenderPassImpl(vk::Device, const rhi::RenderPass::Descriptor&,
                   const RenderPassInfo&);
    ~RenderPassImpl();

    const RenderPass::Descriptor& GetDescriptor() const override {
        return descriptor_;
    }

    vk::RenderPass renderPass;
    RenderPassInfo renderPassInfo;

private:
    vk::Device dev_;
    rhi::RenderPass::Descriptor descriptor_;
};

}  // namespace nickel::rhi::vulkan