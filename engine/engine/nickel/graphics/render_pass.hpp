#pragma once

namespace nickel::graphics {

class RenderPassImpl;

class RenderPass final {
public:
    struct Descriptor {
        std::vector<VkAttachmentDescription> attachments;
        std::vector<VkSubpassDescription> subpasses;
        std::vector<VkSubpassDependency> dependencies;
    };

    explicit RenderPass(RenderPassImpl*);
    RenderPass(const RenderPass&);
    RenderPass(RenderPass&&) noexcept;
    RenderPass& operator=(const RenderPass&) noexcept;
    RenderPass& operator=(RenderPass&&) noexcept;
    ~RenderPass();

    const RenderPassImpl& Impl() const noexcept;
    RenderPassImpl& Impl() noexcept;

private:
    RenderPassImpl* m_impl;
};

}  // namespace nickel::graphics