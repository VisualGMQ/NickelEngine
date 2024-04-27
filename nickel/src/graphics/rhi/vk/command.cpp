#include "graphics/rhi/vk/command.hpp"
#include "graphics/rhi/vk/bind_group.hpp"
#include "graphics/rhi/vk/buffer.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/pipeline_layout.hpp"
#include "graphics/rhi/vk/texture.hpp"
#include "graphics/rhi/vk/texture_view.hpp"

namespace nickel::rhi::vulkan {

RenderPassEncoderImpl::RenderPassEncoderImpl(DeviceImpl& dev,
                                             vk::CommandBuffer cmd,
                                             RenderPassImpl& renderPass)
    : dev_{dev}, cmd_{cmd}, renderPass_{renderPass} {}

void RenderPassEncoderImpl::Draw(uint32_t vertexCount, uint32_t instanceCount,
                                 uint32_t firstVertex, uint32_t firstInstance) {
    cmd_.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void RenderPassEncoderImpl::DrawIndexed(uint32_t indexCount,
                                        uint32_t instanceCount,
                                        uint32_t firstIndex,
                                        uint32_t baseVertex,
                                        uint32_t firstInstance) {
    cmd_.drawIndexed(indexCount, instanceCount, firstIndex, baseVertex,
                     firstInstance);
}

void RenderPassEncoderImpl::SetVertexBuffer(uint32_t slot, Buffer buffer,
                                            uint64_t offset, uint64_t size) {
    // TODO: size not use!
    cmd_.bindVertexBuffers(
        slot, static_cast<const BufferImpl*>(buffer.Impl())->buffer, offset);
}

void RenderPassEncoderImpl::SetIndexBuffer(Buffer buffer, IndexType type,
                                           uint32_t offset, uint32_t size) {
    cmd_.bindIndexBuffer(static_cast<const BufferImpl*>(buffer.Impl())->buffer,
                         offset, IndexType2Vk(type));
}

void RenderPassEncoderImpl::SetBindGroup(BindGroup group) {
    auto bindGroup = static_cast<BindGroupImpl*>(group.Impl());
    bindGroup->Transformlayouts();
    cmd_.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        static_cast<const PipelineLayoutImpl*>(pipeline_.GetLayout().Impl())
            ->layout,
        0, bindGroup->sets[dev_.curImageIndex], {});
}

void RenderPassEncoderImpl::SetBindGroup(
    BindGroup group, const std::vector<uint32_t>& dynamicOffset) {
    auto bindGroup = static_cast<const BindGroupImpl*>(group.Impl());
    cmd_.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        static_cast<const PipelineLayoutImpl*>(pipeline_.GetLayout().Impl())
            ->layout,
        0, bindGroup->sets[dev_.curImageIndex], dynamicOffset);
}

void RenderPassEncoderImpl::SetPipeline(RenderPipeline pipeline) {
    pipeline_ = pipeline;

    auto impl = static_cast<RenderPipelineImpl*>(pipeline.Impl());

    // TODO: change this to renderpass compatible check
    if (impl->defaultRenderPass != GetRenderPass().renderPass) {
        *impl = RenderPipelineImpl{dev_, impl->GetDescriptor(),
                                   renderPass_.renderPass};
    }

    cmd_.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        static_cast<const RenderPipelineImpl*>(pipeline.Impl())->pipeline);
}

void RenderPassEncoderImpl::SetPushConstant(ShaderStage stage, const void* value,
                                            uint32_t offset, uint32_t size) {
    cmd_.pushConstants(
        static_cast<const PipelineLayoutImpl*>(
            static_cast<const RenderPipelineImpl*>(pipeline_.Impl())
                ->GetLayout()
                .Impl())
            ->layout,
        ShaderStage2Vk(stage), offset, size, value);
}

void RenderPassEncoderImpl::SetViewport(float x, float y, float width, float height) {
    vk::Viewport viewport{x, y, width, height, 0, 1};
    cmd_.setViewport(0, viewport);
    vk::Rect2D scissor{vk::Offset2D(x, y), vk::Extent2D(width, height)};
    cmd_.setScissor(0, scissor);
}

void RenderPassEncoderImpl::End() {
    cmd_.endRenderPass();
}

RenderPassImpl& RenderPassEncoderImpl::GetRenderPass() const {
    return renderPass_;
}

bool isSameRenderPass(const RenderPass::Descriptor& desc1,
                      const RenderPass::Descriptor& desc2) {
    if (desc1.renderArea != desc2.renderArea) {
        return false;
    }

    if (desc1.depthStencilAttachment.has_value() !=
        desc2.depthStencilAttachment.has_value()) {
        return false;
    }

    if (desc1.colorAttachments.size() != desc2.colorAttachments.size()) {
        return false;
    }

    for (int i = 0; i < desc1.colorAttachments.size(); i++) {
        auto& att1 = desc1.colorAttachments[i];
        auto& att2 = desc2.colorAttachments[i];

        if (att1.loadOp != att2.loadOp || att1.storeOp != att2.storeOp ||
            /*att1.view != att2.view ||*/ att1.clearValue != att2.clearValue ||
            att1.resolveTarget != att2.resolveTarget) {
            return false;
        }
    }

    if (desc1.depthStencilAttachment && desc2.depthStencilAttachment) {
        if (desc1.depthStencilAttachment->view !=
                desc2.depthStencilAttachment->view ||
            desc1.depthStencilAttachment->depthReadOnly !=
                desc2.depthStencilAttachment->depthReadOnly ||
            desc1.depthStencilAttachment->depthClearValue !=
                desc2.depthStencilAttachment->depthClearValue ||
            desc1.depthStencilAttachment->depthLoadOp !=
                desc2.depthStencilAttachment->depthLoadOp ||
            desc1.depthStencilAttachment->depthStoreOp !=
                desc2.depthStencilAttachment->depthStoreOp ||
            desc1.depthStencilAttachment->stencilReadOnly !=
                desc2.depthStencilAttachment->stencilReadOnly ||
            desc1.depthStencilAttachment->stencilClearValue !=
                desc2.depthStencilAttachment->stencilClearValue ||
            desc1.depthStencilAttachment->stencilLoadOp !=
                desc2.depthStencilAttachment->stencilLoadOp ||
            desc1.depthStencilAttachment->stencilStoreOp !=
                desc2.depthStencilAttachment->stencilStoreOp) {
            return false;
        }
    }

    return true;
}

bool isSameFramebuffer(const std::vector<vk::ImageView>& view,
                       const Framebuffer& fbo) {
    auto& f1 = *static_cast<const FramebufferImpl*>(fbo.Impl());

    return view == f1.Views();
}

RenderPassEncoder CommandEncoderImpl::BeginRenderPass(
    const RenderPass::Descriptor& desc) {
    if (type_ != CmdType::RenderPass && type_ != CmdType::None) {
        LOGE(log_tag::Vulkan, "record non-compatible commands");
    }
    type_ = CmdType::RenderPass;
    auto cmdBuf = static_cast<CommandBufferImpl*>(cmdBuf_);

    std::vector<vk::ImageView> views;
    const TextureViewImpl* view =
        desc.colorAttachments.empty()
            ? nullptr
            : static_cast<const TextureViewImpl*>(
                  desc.colorAttachments[0].view.Impl());

    for (auto& colorAtt : desc.colorAttachments) {
        views.push_back(
            static_cast<const vulkan::TextureViewImpl*>(colorAtt.view.Impl())
                ->GetView());
    }
    if (desc.depthStencilAttachment) {
        auto depthView = static_cast<const vulkan::TextureViewImpl*>(
            desc.depthStencilAttachment->view.Impl());
        views.push_back(depthView->GetView());

        if (!view) {
            view = depthView;
        }
    }

    std::vector<vk::ClearValue> clearValues;
    for (auto& colorAtt : desc.colorAttachments) {
        if (colorAtt.loadOp == AttachmentLoadOp::Clear) {
            vk::ClearValue value;
            value.setColor(colorAtt.clearValue);
            clearValues.emplace_back(value);
        }

        auto texture =
            static_cast<TextureImpl*>(colorAtt.view.Texture().Impl());
        // record layout transition
        for (int i = 0; i < texture->Extent().depthOrArrayLayers; i++) {
            cmdBuf->AddLayoutTransition(
                texture->layouts[0],
                GetImageLayoutAfterSubpass(colorAtt.view.Format()));
        }
    }

    if (desc.depthStencilAttachment) {
        if (desc.depthStencilAttachment->depthLoadOp == AttachmentLoadOp::Clear) {
            vk::ClearValue value;
            value.depthStencil
                .setDepth(desc.depthStencilAttachment->depthClearValue)
                .setStencil(desc.depthStencilAttachment->stencilClearValue);
            clearValues.emplace_back(value);
        }

        auto texture = static_cast<TextureImpl*>(
            desc.depthStencilAttachment->view.Texture().Impl());
        // record layout transition
        for (int i = 0; i < texture->Extent().depthOrArrayLayers; i++) {
            cmdBuf->AddLayoutTransition(
                texture->layouts[0], GetDepthStencilLayoutAfterSubpass(desc));
        }
    }

    RenderPass renderPass;
    for (auto& rp : dev_.renderPasses) {
        if (isSameRenderPass(desc, rp.GetDescriptor())) {
            renderPass = rp;
            break;
        }
    }

    if (!renderPass) {
        renderPass =
            dev_.renderPasses.emplace_back(APIPreference::Vulkan, dev_, desc);
    }

    vk::Framebuffer fbo;
    for (auto& f : dev_.framebuffers) {
        if (isSameFramebuffer(views, f)) {
            fbo = static_cast<const FramebufferImpl*>(f.Impl())->fbo;
        }
    }

    if (!fbo) {
        fbo = static_cast<const FramebufferImpl*>(
                  dev_.framebuffers
                      .emplace_back(new FramebufferImpl(
                          dev_.device, views, view->Texture().Extent(),
                          static_cast<const RenderPassImpl*>(renderPass.Impl())
                              ->renderPass))
                      .Impl())
                  ->fbo;
    }

    auto renderPassImpl = static_cast<RenderPassImpl*>(renderPass.Impl());
    vk::RenderPassBeginInfo info;
    info.setRenderArea(desc.renderArea ?
                        vk::Rect2D{{desc.renderArea->offset.x, desc.renderArea->offset.y},
                                    {desc.renderArea->extent.width, desc.renderArea->extent.height}} :
                        vk::Rect2D{{0, 0},
                                    {dev_.swapchain.ImageInfo().extent.width, dev_.swapchain.ImageInfo().extent.height}})
        .setClearValues(clearValues)
        .setRenderPass(renderPassImpl->renderPass)
        .setFramebuffer(fbo);
    buf.beginRenderPass(info, vk::SubpassContents::eInline);

    return RenderPassEncoder{
        new RenderPassEncoderImpl{dev_, buf, *renderPassImpl}
    };
}

CommandEncoderImpl::CommandEncoderImpl(DeviceImpl& dev, vk::CommandPool pool)
    : dev_{dev}, pool{pool} {
    vk::CommandBufferAllocateInfo info;
    info.setCommandPool(pool).setCommandBufferCount(1);
    std::vector<vk::CommandBuffer> cmds;
    VK_CALL(cmds, dev.device.allocateCommandBuffers(info));
    buf = cmds[0];

    cmdBuf_ = new CommandBufferImpl(buf);

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    VK_CALL_NO_VALUE(buf.begin(beginInfo));
}

CommandEncoderImpl::~CommandEncoderImpl() {
    if (pool) {
        delete cmdBuf_;
        dev_.device.freeCommandBuffers(pool, buf);
        dev_.device.resetCommandPool(pool);
    }
}

void CommandEncoderImpl::CopyBufferToBuffer(const Buffer& src,
                                            uint64_t srcOffset,
                                            const Buffer& dst,
                                            uint64_t dstOffset, uint64_t size) {
    if (type_ != CmdType::CopyData && type_ != CmdType::None) {
        LOGE(log_tag::Vulkan, "record non-compatible commands");
    }
    type_ = CmdType::CopyData;

    // maybe we need barrier
    vk::BufferCopy region;
    region.setSize(size).setSrcOffset(srcOffset).setDstOffset(dstOffset);
    buf.copyBuffer(static_cast<const vulkan::BufferImpl*>(src.Impl())->buffer,
                    static_cast<const vulkan::BufferImpl*>(dst.Impl())->buffer,
                    region);
}

void CommandEncoderImpl::CopyBufferToTexture(
    const CommandEncoder::BufTexCopySrc& src,
    const CommandEncoder::BufTexCopyDst& dst, const Extent3D& copySize) {
    if (type_ != CmdType::CopyData && type_ != CmdType::None) {
        LOGE(log_tag::Vulkan, "record non-compatible commands");
    }
    type_ = CmdType::CopyData;

    auto& buffer =
        static_cast<const vulkan::BufferImpl*>(src.buffer.Impl())->buffer;
    auto& image = *static_cast<vulkan::TextureImpl*>(dst.texture.Impl());

    bool shouldCopySplit = false;

    for (int i = dst.origin.z; i < (int)copySize.depthOrArrayLayers - 1; i++) {
        if (image.layouts[i] != image.layouts[i + 1]) {
            shouldCopySplit = true;
            break;
        }
    }

    if (shouldCopySplit) {
        for (int i = dst.origin.z; i < copySize.depthOrArrayLayers; i++) {
            auto oldLayout = image.layouts[i];

            auto aspect = DetermineTextureAspect(dst.aspect, image.Format());
            vk::ImageSubresourceRange range;
            range.setAspectMask(aspect)
                .setBaseArrayLayer(i)
                .setLevelCount(1)
                .setLayerCount(1)
                .setBaseMipLevel(dst.miplevel);
            vk::ImageMemoryBarrier barrier;
            barrier.setImage(image.GetImage())
                .setOldLayout(oldLayout)
                .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
                .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
                .setSrcQueueFamilyIndex(dev_.queueIndices.graphicsIndex.value())
                .setSubresourceRange(range);
            buf.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                                 vk::PipelineStageFlagBits::eTransfer,
                                 vk::DependencyFlagBits::eByRegion, {}, {},
                                 barrier);

            vk::ImageSubresourceLayers layers;
            layers.setAspectMask(aspect)
                .setBaseArrayLayer(i)
                .setLayerCount(1)
                .setMipLevel(dst.miplevel);
            vk::BufferImageCopy copyInfo;
            copyInfo
                .setBufferOffset(src.offset + dst.texture.Extent().width *
                                                  dst.texture.Extent().height *
                                                  (i - dst.origin.z))
                .setImageOffset(vk::Offset3D(dst.origin.x, dst.origin.y, 0))
                .setBufferImageHeight(src.rowsPerImage)
                .setBufferRowLength(src.rowLength)
                .setImageExtent({copySize.width, copySize.height, 1})
                .setImageSubresource(layers);
            buf.copyBufferToImage(buffer, image.GetImage(),
                                   vk::ImageLayout::eTransferDstOptimal,
                                   copyInfo);

            barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                .setDstAccessMask(vk::AccessFlagBits::eShaderRead);
            buf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                 vk::PipelineStageFlagBits::eFragmentShader,
                                 vk::DependencyFlagBits::eByRegion, {}, {},
                                 barrier);

            // record layout transition
            static_cast<CommandBufferImpl*>(cmdBuf_)
                ->AddLayoutTransition(oldLayout, barrier.newLayout);
        }
    } else {
        auto oldLayout = image.layouts[dst.origin.z];

        auto aspect = DetermineTextureAspect(dst.aspect, image.Format());
        vk::ImageSubresourceRange range;
        range.setAspectMask(aspect)
            .setBaseArrayLayer(dst.origin.z)
            .setLevelCount(1)
            .setLayerCount(copySize.depthOrArrayLayers)
            .setBaseMipLevel(dst.miplevel);
        vk::ImageMemoryBarrier barrier;
        barrier.setImage(image.GetImage())
            .setOldLayout(oldLayout)
            .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
            .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setSrcQueueFamilyIndex(dev_.queueIndices.graphicsIndex.value())
            .setSubresourceRange(range);
        buf.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                             vk::PipelineStageFlagBits::eTransfer,
                             vk::DependencyFlagBits::eByRegion, {}, {},
                             barrier);

        vk::ImageSubresourceLayers layers;
        layers.setAspectMask(aspect)
            .setBaseArrayLayer(dst.origin.z)
            .setLayerCount(copySize.depthOrArrayLayers)
            .setMipLevel(dst.miplevel);
        vk::BufferImageCopy copyInfo;
        copyInfo.setBufferOffset(src.offset)
            .setImageOffset(vk::Offset3D(dst.origin.x, dst.origin.y, 0))
            .setBufferImageHeight(src.rowsPerImage)
            .setBufferRowLength(src.rowLength)
            .setImageExtent({copySize.width, copySize.height, 1})
            .setImageSubresource(layers);
        buf.copyBufferToImage(buffer, image.GetImage(),
                               vk::ImageLayout::eTransferDstOptimal, copyInfo);

        barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead);
        buf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                             vk::PipelineStageFlagBits::eFragmentShader,
                             vk::DependencyFlagBits::eByRegion, {}, {},
                             barrier);

        // record layout transition
        for (int i = dst.origin.z; i < copySize.depthOrArrayLayers; i++) {
            static_cast<CommandBufferImpl*>(cmdBuf_)
                ->AddLayoutTransition(image.layouts[i], barrier.newLayout);
        }
    }
}

CommandBuffer CommandEncoderImpl::Finish() {
    VK_CALL_NO_VALUE(buf.end());
    static_cast<CommandBufferImpl*>(cmdBuf_)->type_ = type_;
    return CommandBuffer{cmdBuf_};
}

}  // namespace nickel::rhi::vulkan