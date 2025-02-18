#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/graphics/lowlevel/enums.hpp"
#include <optional>

namespace nickel::graphics {

class RenderPassImpl;

class NICKEL_API RenderPass final {
public:
    struct Descriptor {
        struct SubpassDependency {
            static constexpr uint32_t ExternalSubpass = (~0U);
            
            uint32_t m_src_subpass{};
            uint32_t m_dst_subpass{};
            Flags<PipelineStage> m_src_stage_mask;
            Flags<PipelineStage> m_dst_stage_mask;
            Flags<Access> m_src_access_mask;
            Flags<Access> m_dst_access_mask;
            Flags<Dependency> m_dependency_flags;
        };

        struct AttachmentDescription {
            Format m_format;
            SampleCount m_samples;
            AttachmentLoadOp m_load_op;
            AttachmentStoreOp m_store_op;
            AttachmentLoadOp m_stencil_load_op;
            AttachmentStoreOp m_stencil_store_op;
            ImageLayout m_initial_layout;
            ImageLayout m_final_layout;
        };

        struct AttachmentReference {
            uint32_t m_attachment{};
            ImageLayout m_layout = ImageLayout::Undefined;
        };

        struct SubpassDescription {
            PipelineBindPoint m_pipeline_bind_point;
            std::vector<AttachmentReference> m_input_attachments;
            std::vector<AttachmentReference> m_color_attachments;
            std::vector<AttachmentReference> m_resolve_attachments;
            std::optional<AttachmentReference> m_depth_stencil_attachment;
            std::vector<uint32_t> m_preserve_attachments;
        };

        std::vector<AttachmentDescription> m_attachments;
        std::vector<SubpassDescription> m_subpasses;
        std::vector<SubpassDependency> m_dependencies;
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