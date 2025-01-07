#pragma once
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class RenderPassImpl;

class NICKEL_API RenderPass final {
public:
    struct Descriptor {
        std::vector<VkAttachmentDescription> attachments;
        std::vector<VkSubpassDescription> subpasses;
        std::vector<VkSubpassDependency> dependencies;
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