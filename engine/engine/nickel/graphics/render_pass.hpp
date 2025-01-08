#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/graphics/enums.hpp"

namespace nickel::graphics {

class RenderPassImpl;

class NICKEL_API RenderPass final {
public:
    struct Descriptor {
        struct SubpassDependency {
            uint32_t srcSubpass{};
            uint32_t dstSubpass{};
            Flags<PipelineStage> srcStageMask;
            Flags<PipelineStage> dstStageMask;
            Flags<Access> srcAccessMask;
            Flags<Access> dstAccessMask;
            Flags<Dependency> dependencyFlags;
        };

        struct AttachmentDescription {
            Format format;
            SampleCount samples;
            AttachmentLoadOp loadOp;
            AttachmentStoreOp storeOp;
            AttachmentLoadOp stencilLoadOp;
            AttachmentStoreOp stencilStoreOp;
            ImageLayout initialLayout;
            ImageLayout finalLayout;
        };

        struct AttachmentReference {
            uint32_t attachment{};
            ImageLayout layout = ImageLayout::Undefined;
        };

        struct SubpassDescription {
            PipelineBindPoint pipelineBindPoint;
            std::vector<AttachmentReference> inputAttachments;
            std::vector<AttachmentReference> colorAttachments;
            std::vector<AttachmentReference> resolveAttachments;
            std::optional<AttachmentReference> depthStencilAttachment;
            std::vector<uint32_t> preserveAttachments;
        };

        std::vector<AttachmentDescription> attachments;
        std::vector<SubpassDescription> subpasses;
        std::vector<SubpassDependency> dependencies;
    };

    RenderPass() = default;
    explicit RenderPass(RenderPassImpl*);
    RenderPass(const RenderPass&);
    RenderPass(RenderPass&&) noexcept;
    RenderPass& operator=(const RenderPass&) noexcept;
    RenderPass& operator=(RenderPass&&) noexcept;
    ~RenderPass();

    const RenderPassImpl& Impl() const noexcept;
    RenderPassImpl& Impl() noexcept;

    operator bool() const noexcept;
    void Release();

private:
    RenderPassImpl* m_impl{};
};

}  // namespace nickel::graphics