#pragma once

#include "pch.hpp"
#include "rhi/enums.hpp"

namespace nickel::rhi {

struct AttachmentDescription final {
    ImageLayout initLayout;
    ImageLayout finalLayout;
    AttachmentLoadOp loadOp;
    AttachmentStoreOp storeOp;
    AttachmentLoadOp stencilLoadOp;
    AttachmentStoreOp stencilStoreOp;
    SampleCountFlag sampleCountFlag;
    Format format;
};

struct AttachmentReference final {
    uint32_t attachmentIndex;
    ImageLayout layout;
};

struct SubpassDescription final {
    PipelineBindPoint pipelineBindPoint;
    std::vector<AttachmentReference> colorAttachments;
    std::vector<AttachmentReference> inputAttachments;
    std::vector<AttachmentReference> resolveAttachments;
    std::optional<AttachmentReference> depthStencilAttachment;
    std::vector<uint32_t> preserveAttachments;
};

struct SubpassDependency final {
    uint32_t srcSubpassIndex;
    uint32_t dstSubpassIndex;
    PipelineStageFlags srcStageMask;
    PipelineStageFlags dstStageMask;
    AccessFlags srcAccessMask;
    AccessFlags dstAccessMask;
};

class RenderPass {
public:
    virtual ~RenderPass() = default;
};

}  // namespace nickel::rhi