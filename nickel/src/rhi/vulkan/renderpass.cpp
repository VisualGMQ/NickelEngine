#include "rhi/vulkan/renderpass.hpp"
#include "rhi/vulkan/device.hpp"
#include "rhi/vulkan/enum_convert.hpp"

namespace nickel::rhi::vulkan {

inline vk::AttachmentReference attachmentRef2Vk(
    const AttachmentReference& ref) {
    vk::AttachmentReference reference;
    reference.setLayout(ImageLayout2Vk(ref.layout))
        .setAttachment(ref.attachmentIndex);
    return reference;
}

inline std::vector<vk::AttachmentReference> attachRefs2Vk(
    const std::vector<AttachmentReference>& refs) {
    std::vector<vk::AttachmentReference> references;
    for (auto& ref : refs) {
        references.emplace_back(attachmentRef2Vk(ref));
    }
    return references;
}

struct AttachmentRef {
    using Range = std::pair<size_t, size_t>;
    Range colorAttachments;
    Range inputAttachments;
    Range resolveAttachments;
    std::optional<uint32_t> depthStencilIndex;
    const std::vector<uint32_t>* preserveAttachments = nullptr;
};

AttachmentRef::Range addAndRecordRef(
    std::vector<vk::AttachmentReference>& refs,
    const std::vector<AttachmentReference> rhiRefs) {
    AttachmentRef::Range range;
    range.first = refs.size();
    for (auto& ref : rhiRefs) {
        refs.emplace_back(attachmentRef2Vk(ref));
    }
    range.second = refs.size();
    return range;
}

RenderPass::RenderPass(
    Device* device, const std::vector<AttachmentDescription>& attachmentDescs,
    const std::vector<SubpassDescription>& subpasses,
    const std::vector<SubpassDependency>& subpassDeps)
    : device_(device) {
    std::vector<vk::AttachmentDescription> attDescs;
    for (auto& att : attachmentDescs) {
        vk::AttachmentDescription desc;
        desc.setFormat(Format2Vk(att.format))
            .setFinalLayout(ImageLayout2Vk(att.finalLayout))
            .setInitialLayout(ImageLayout2Vk(att.initLayout))
            .setLoadOp(AttachmentLoadOp2Vk(att.loadOp))
            .setStoreOp(AttachmentStoreOp2Vk(att.storeOp))
            .setStencilLoadOp(AttachmentLoadOp2Vk(att.stencilLoadOp))
            .setStencilStoreOp(AttachmentStoreOp2Vk(att.stencilStoreOp))
            .setSamples(SampleCountFlag2Vk(att.sampleCountFlag));
        attDescs.emplace_back(desc);
    }

    // convert rhi data to vulkan
    std::vector<vk::AttachmentReference> references;
    std::vector<AttachmentRef> refRanges;
    for (auto& subpass : subpasses) {
        AttachmentRef ref;
        ref.colorAttachments =
            addAndRecordRef(references, subpass.colorAttachments);
        ref.inputAttachments =
            addAndRecordRef(references, subpass.inputAttachments);
        ref.resolveAttachments =
            addAndRecordRef(references, subpass.resolveAttachments);

        if (subpass.depthStencilAttachment) {
            references.emplace_back(
                attachmentRef2Vk(subpass.depthStencilAttachment.value()));
            ref.depthStencilIndex = references.size();
        }

        if (!subpass.preserveAttachments.empty()) {
            ref.preserveAttachments = &subpass.preserveAttachments;
        }
    }

    // set subpass description
    std::vector<vk::SubpassDescription> subpassDescs;
    for (auto& range : refRanges) {
        vk::SubpassDescription desc;

        size_t count =
            range.colorAttachments.second - range.colorAttachments.first;
        desc.setColorAttachmentCount(count);
        if (count) {
            desc.setPColorAttachments(references.data() +
                                      range.colorAttachments.first);
        }

        count = range.inputAttachments.second - range.inputAttachments.first;
        desc.setInputAttachmentCount(count);
        if (count) {
            desc.setPInputAttachments(references.data() +
                                      range.inputAttachments.first);
        }

        size_t resolveCount =
            range.resolveAttachments.second - range.resolveAttachments.first;
        if (resolveCount > 0) {
            desc.setPResolveAttachments(references.data() +
                                        range.resolveAttachments.first);
            desc.setColorAttachmentCount(resolveCount);
        }
        if (range.preserveAttachments && !range.preserveAttachments->empty()) {
            desc.setPreserveAttachments(*range.preserveAttachments);
        }
        if (range.depthStencilIndex) {
            desc.setPDepthStencilAttachment(references.data() +
                                            range.depthStencilIndex.value());
        }

        subpassDescs.emplace_back(std::move(desc));
    }

    // convert subpass dependency
    std::vector<vk::SubpassDependency> subpassDependencies;
    for (auto& dep : subpassDeps) {
        vk::SubpassDependency dependency;
        dependency.setDstSubpass(dep.dstSubpassIndex)
            .setSrcSubpass(dep.srcSubpassIndex)
            .setDstAccessMask(AccessFlags2Vk(dep.dstAccessMask))
            .setSrcAccessMask(AccessFlags2Vk(dep.srcAccessMask))
            .setDstStageMask(PipelineStageFlags2Vk(dep.dstStageMask))
            .setSrcStageMask(PipelineStageFlags2Vk(dep.srcStageMask));
        subpassDependencies.emplace_back(std::move(dependency));
    }

    // set renderpass createinfo
    vk::RenderPassCreateInfo createInfo;
    createInfo.setSubpasses(subpassDescs)
        .setAttachments(attDescs)
        .setDependencies(subpassDependencies);

    renderPass_ = device_->Raw().createRenderPass(createInfo);
    if (!renderPass_) {
        LOGW(log_tag::Vulkan, "create renderpass failed");
    }
}

RenderPass::~RenderPass() {
    if (device_ && renderPass_) {
        device_->Raw().destroyRenderPass(renderPass_);
    }
}

}  // namespace nickel::rhi::vulkan