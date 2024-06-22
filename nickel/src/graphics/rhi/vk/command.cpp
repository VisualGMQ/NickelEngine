#include "graphics/rhi/vk/command.hpp"
#include "graphics/rhi/vk/bind_group.hpp"
#include "graphics/rhi/vk/buffer.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/framebuffer.hpp"
#include "graphics/rhi/vk/pipeline_layout.hpp"
#include "graphics/rhi/vk/texture.hpp"
#include "graphics/rhi/vk/texture_view.hpp"

namespace nickel::rhi::vulkan {

bool isSameFramebuffer(const std::vector<TextureView>& views,
                       const Framebuffer& fbo) {
    auto& views2 = static_cast<const FramebufferImpl*>(fbo.Impl())->Views();

    return std::equal(
        views.begin(), views.end(), views2.begin(), views2.end(),
        [](TextureView view1, TextureView view2) {
            return static_cast<TextureViewImpl*>(view1.Impl())->GetView() ==
                   static_cast<TextureViewImpl*>(view2.Impl())->GetView();
        });
}

struct CmdAnalyzer final {
    CmdAnalyzer(CommandEncoderImpl& encoder, DeviceImpl& device)
        : encoder_{encoder}, device_{device} {}

    const std::vector<Command>& Analyze() {
        while (index_ < encoder_.cmds.size()) {
            auto& command = encoder_.cmds[index_];
            if (auto cmd = std::get_if<CmdCopyBufferToTexture>(&command); cmd) {
                analyzeCopyBuffer2Image(*cmd);
            } else if (auto cmd = std::get_if<CmdBeginRenderPass>(&command);
                       cmd) {
                analyzeBeginRenderPass(*cmd);
            } else if (auto cmd = std::get_if<CmdSetBindGroup>(&command); cmd) {
                analyzeSetBindGroup(*cmd);
            } else{
                cmds_.emplace_back(command);
            }
            index_++;
        }

        return cmds_;
    }

    bool HasPresent() const { return hasPresent_; }

    CommandEncoderImpl& encoder_;

private:
    int index_ = 0;
    DeviceImpl& device_;
    std::vector<Command> cmds_;
    std::optional<size_t> lastRenderPassCmdIndex_;
    bool hasPresent_ = false;

    bool isSameViews(const std::vector<TextureView>& views,
                     const RenderPass::Descriptor& desc) {
        if (desc.colorAttachments.size() +
                (desc.depthStencilAttachment.has_value() ? 1 : 0) !=
            views.size()) {
            return false;
        }

        bool isSame = false;

        for (auto view : views) {
            for (auto att : desc.colorAttachments) {
                if (view == att.view) {
                    isSame = true;
                    break;
                }
            }

            if (!isSame && desc.depthStencilAttachment) {
                isSame = desc.depthStencilAttachment->view == view;
            }

            if (!isSame) {
                break;
            }
        }

        return isSame;
    }

    void analyzeCopyBuffer2Image(const CmdCopyBufferToTexture& cmd) {
        auto texture = static_cast<TextureImpl*>(cmd.dst.texture.Impl());
        auto buffer = static_cast<BufferImpl*>(cmd.src.buffer.Impl());


        for (int i = 0; i < cmd.copySize.depthOrArrayLayers; i++) {
            int layer = cmd.dst.origin.z + i;
            auto& layout = texture->layouts[layer];
            if (layout == vk::ImageLayout::eTransferDstOptimal) {
                continue;
            }

            auto aspect =
                DetermineTextureAspectByFormat(texture->Format());
            vk::ImageSubresourceRange range;
            range.setAspectMask(aspect)
                .setBaseArrayLayer(layer)
                .setLevelCount(1)
                .setLayerCount(1)
                .setBaseMipLevel(cmd.dst.miplevel);
            vk::ImageMemoryBarrier barrier;
            barrier.setImage(texture->GetImage())
                .setOldLayout(layout)
                .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
                .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
                .setSrcQueueFamilyIndex(
                    device_.queueIndices.graphicsIndex.value())
                .setSubresourceRange(range);

            CmdImageBarrier command;
            command.srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
            command.dstStage = vk::PipelineStageFlagBits::eTransfer;
            command.imageMemBarrier = barrier;

            cmds_.emplace_back(command);
            cmds_.emplace_back(cmd);

            layout = vk::ImageLayout::eTransferDstOptimal;
        }
    }

    void analyzeBeginRenderPass(const CmdBeginRenderPass& cmd) {
        auto clearValues = analyzeClearColorValue(cmd.desc);

        vk::Rect2D renderArea;
        if (cmd.desc.renderArea) {
            renderArea.extent.setWidth(cmd.desc.renderArea->extent.width);
            renderArea.extent.setHeight(cmd.desc.renderArea->extent.width);
            renderArea.offset.setX(cmd.desc.renderArea->offset.x);
            renderArea.offset.setY(cmd.desc.renderArea->offset.y);
        } else {
            if (!cmd.desc.colorAttachments.empty()) {
                auto extent =
                    cmd.desc.colorAttachments[0].view.Texture().Extent();
                renderArea.extent.setWidth(extent.width);
                renderArea.extent.setHeight(extent.height);
                renderArea.offset.setX(0);
                renderArea.offset.setY(0);
            } else if (cmd.desc.depthStencilAttachment) {
                auto extent =
                    cmd.desc.depthStencilAttachment->view.Texture().Extent();
                renderArea.extent.setWidth(extent.width);
                renderArea.extent.setHeight(extent.height);
                renderArea.offset.setX(0);
                renderArea.offset.setY(0);
            } else {
                renderArea.offset.setX(0);
                renderArea.offset.setY(0);
                renderArea.extent.setWidth(
                    device_.swapchain.ImageInfo().extent.width);
                renderArea.extent.setHeight(
                    device_.swapchain.ImageInfo().extent.height);
            }
        }

        // if (!lastRenderPassCmdIndex_ ||
        //     !canMergeRenderPass(std::get<CmdBeginRenderPass>(
        //                             cmds_[lastRenderPassCmdIndex_.value()]),
        //                         cmd.desc, renderArea)) {
            bool hasPresentImage = std::any_of(
                cmd.desc.colorAttachments.begin(),
                cmd.desc.colorAttachments.end(), [](auto&& attachment) {
                    return attachment.view.Format() ==
                           TextureFormat::Presentation;
                });
            auto descriptions = analyzeDescription(cmd.desc);
            auto views = gatherImageViews(cmd.desc);
            auto subpass = analyzeSubpass(views, cmd.desc);
            vk::SubpassDependency dep;
            dep.setSrcSubpass(VK_SUBPASS_EXTERNAL)
                .setDstSubpass(0)
                .setSrcAccessMask(vk::AccessFlagBits::eNone)
                .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                .setSrcStageMask(
                    vk::PipelineStageFlagBits::eColorAttachmentOutput)
                .setDstStageMask(
                    vk::PipelineStageFlagBits::eColorAttachmentOutput);

            if (lastRenderPassCmdIndex_ && hasPresentImage) {
                auto& lastRenderpass = std::get<CmdBeginRenderPass>(
                    cmds_[lastRenderPassCmdIndex_.value()]);
                for (auto& desc :
                     lastRenderpass.analyzedRenderPassInfo.descriptions) {
                    if (desc.finalLayout == vk::ImageLayout::ePresentSrcKHR) {
                        desc.setFinalLayout(
                            vk::ImageLayout::eColorAttachmentOptimal);
                    }
                }

                for (auto& desc : descriptions) {
                    if (desc.initialLayout == vk::ImageLayout::ePresentSrcKHR) {
                        desc.setInitialLayout(
                            vk::ImageLayout::eColorAttachmentOptimal);
                    }
                }
            }

            RenderPassInfo info;
            info.descriptions = std::move(descriptions);
            info.subpasses.emplace_back(std::move(subpass));
            info.dependencies.emplace_back(std::move(dep));

            CmdBeginRenderPass newCmd;
            newCmd.desc = cmd.desc;
            newCmd.renderArea = renderArea;
            newCmd.views = std::move(views);
            newCmd.clearValues = std::move(clearValues);
            newCmd.analyzedRenderPassInfo = std::move(info);

            cmds_.emplace_back(std::move(newCmd));

            lastRenderPassCmdIndex_ = cmds_.size() - 1;
            return;

        /*
        }

        cmds_.erase(
            std::remove_if(
                cmds_.begin() + lastRenderPassCmdIndex_.value(), cmds_.end(),
                [](Command cmd) {
                    return std::get_if<CmdEndRenderPass>(&cmd) != nullptr;
                }),
            cmds_.end());

        cmds_.emplace_back(CmdNextSubpass{});

        auto lastRenderPass = std::get_if<CmdBeginRenderPass>(
            &cmds_[lastRenderPassCmdIndex_.value()]);

        auto subpass = analyzeSubpass(lastRenderPass->views, cmd.desc);
        auto& lastDependency =
            lastRenderPass->analyzedRenderPassInfo.dependencies.back();

        vk::SubpassDependency dep;
        dep.setSrcSubpass(
               lastRenderPass->analyzedRenderPassInfo.subpasses.size() - 1)
            .setDstSubpass(
                lastRenderPass->analyzedRenderPassInfo.subpasses.size())
            .setSrcAccessMask(lastDependency.srcAccessMask)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
            .setSrcStageMask(lastDependency.dstStageMask)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

        lastRenderPass->analyzedRenderPassInfo.subpasses.emplace_back(
            std::move(subpass));
        lastRenderPass->analyzedRenderPassInfo.dependencies.emplace_back(
            std::move(dep));
        */
    }

    bool canMergeRenderPass(const CmdBeginRenderPass& lastRenderPass,
                            const RenderPass::Descriptor& desc,
                            const vk::Rect2D& renderArea) {
        if (renderArea != lastRenderPass.renderArea) {
            return false;
        }

        for (auto&& att : desc.colorAttachments) {
            if (att.loadOp != AttachmentLoadOp::Load) {
                return false;
            }
        }

        return isSameViews(lastRenderPass.views, desc);
    }

    std::vector<TextureView> gatherImageViews(
        const RenderPass::Descriptor& desc) {
        std::vector<TextureView> views;

        for (auto& colorAtt : desc.colorAttachments) {
            views.push_back(colorAtt.view);
        }
        if (desc.depthStencilAttachment) {
            views.push_back(desc.depthStencilAttachment->view);
        }

        return views;
    }

    std::vector<vk::AttachmentDescription> analyzeDescription(
        const RenderPass::Descriptor& desc) {
        std::vector<vk::AttachmentDescription> descriptions;

        size_t attCount = desc.colorAttachments.size() +
                          (desc.depthStencilAttachment.has_value() ? 1 : 0);
        descriptions.reserve(attCount);

        for (int i = 0; i < desc.colorAttachments.size(); i++) {
            auto& colorAtt = desc.colorAttachments[i];
            vk::AttachmentDescription attDesc;

            auto texture = static_cast<vulkan::TextureImpl*>(
                colorAtt.view.Texture().Impl());

            vk::ImageLayout layout = texture->layouts[0];

            if (!lastRenderPassCmdIndex_) {
                layout = vk::ImageLayout::eUndefined;
            }

            attDesc.setInitialLayout(layout)
                .setFinalLayout(
                    GetImageLayoutAfterRenderPass(colorAtt.view.Format()))
                .setFormat(colorAtt.view.Format() == TextureFormat::Presentation
                               ? device_.swapchain.imageInfo.format.format
                               : TextureFormat2Vk(colorAtt.view.Format()))
                .setLoadOp(AttachmentLoadOp2Vk(colorAtt.loadOp))
                .setStoreOp(AttachmentStoreOp2Vk(colorAtt.storeOp))
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eNone)
                .setSamples(
                    SampleCount2Vk(colorAtt.view.Texture().SampleCount()));

            if (texture->Format() == rhi::TextureFormat::Presentation) {
                hasPresent_ = true;
            }

            texture->layouts[0] = attDesc.finalLayout;
            descriptions.push_back(attDesc);
        }

        vk::AttachmentReference depthAttRef;
        if (desc.depthStencilAttachment) {
            vk::AttachmentDescription depthAttach;

            auto texture = static_cast<TextureImpl*>(
                desc.depthStencilAttachment->view.Texture().Impl());

            vk::ImageLayout layout = texture->layouts[0];

            if (!lastRenderPassCmdIndex_) {
                layout = vk::ImageLayout::eUndefined;
            }

            depthAttach
                .setInitialLayout(layout)
                /*
                   TODO: currently we don't enable separateDepthStencilLayouts,
                   so depth & stencil will use one layout
                */
                .setFinalLayout(GetDepthStencilLayoutAfterRenderPass(desc))
                .setFormat(TextureFormat2Vk(
                    desc.depthStencilAttachment->view.Format()))
                .setLoadOp(AttachmentLoadOp2Vk(
                    desc.depthStencilAttachment->depthLoadOp))
                .setStoreOp(AttachmentStoreOp2Vk(
                    desc.depthStencilAttachment->depthStoreOp))
                .setStencilLoadOp(AttachmentLoadOp2Vk(
                    desc.depthStencilAttachment->stencilLoadOp))
                .setStencilStoreOp(AttachmentStoreOp2Vk(
                    desc.depthStencilAttachment->stencilStoreOp))
                .setSamples(SampleCount2Vk(
                    desc.depthStencilAttachment->view.Texture().SampleCount()));

            texture->layouts[0] = depthAttach.finalLayout;
            descriptions.push_back(depthAttach);
        }

        return descriptions;
    }

    SubpassInfo analyzeSubpass(const std::vector<TextureView>& views,
                               const RenderPass::Descriptor& descriptor) {
        SubpassInfo subpass;

        subpass.colorRefs.reserve(descriptor.colorAttachments.size());

        for (int i = 0; i < descriptor.colorAttachments.size(); i++) {
            auto& colorAtt = descriptor.colorAttachments[i];
            vk::AttachmentReference reference;

            for (int i = 0; i < views.size(); i++) {
                if (colorAtt.view == views[i]) {
                    reference.setAttachment(i);
                }
            }

            reference.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
            subpass.colorRefs.push_back(reference);
        }

        vk::AttachmentReference depthAttRef;
        if (descriptor.depthStencilAttachment) {
            vk::AttachmentReference reference;

            for (int i = 0; i < views.size(); i++) {
                if (views[i] == descriptor.depthStencilAttachment->view) {
                    reference.setAttachment(i);
                }
            }

            /*
                TODO: currently we don't enable separateDepthStencilLayouts,
                so depth & stencil will use one layout
            */
            if (descriptor.depthStencilAttachment->depthReadOnly &&
                descriptor.depthStencilAttachment->stencilReadOnly) {
                reference.setLayout(
                    vk::ImageLayout::eDepthStencilReadOnlyOptimal);
            } else {
                reference.setLayout(
                    vk::ImageLayout::eDepthStencilAttachmentOptimal);
            }
            subpass.depthRef = reference;
        }

        return subpass;
    }

    std::vector<vk::ClearValue> analyzeClearColorValue(
        const RenderPass::Descriptor& desc) {
        std::vector<vk::ClearValue> clearValues;

        for (auto& colorAtt : desc.colorAttachments) {
            if (colorAtt.loadOp == AttachmentLoadOp::Clear) {
                vk::ClearValue value;
                value.setColor(colorAtt.clearValue);
                clearValues.emplace_back(value);
            }
        }

        if (desc.depthStencilAttachment) {
            if (desc.depthStencilAttachment->depthLoadOp ==
                AttachmentLoadOp::Clear) {
                vk::ClearValue value;
                value.depthStencil
                    .setDepth(desc.depthStencilAttachment->depthClearValue)
                    .setStencil(desc.depthStencilAttachment->stencilClearValue);
                clearValues.emplace_back(value);
            }
        }
        return clearValues;
    }

    void analyzeSetBindGroup(const CmdSetBindGroup& cmd) {
        auto bindGroup = static_cast<BindGroupImpl*>(cmd.bindgroup.Impl());
        auto bindGroupLayout =
            static_cast<BindGroupLayoutImpl*>(bindGroup->GetLayout().Impl());
        auto& desc = bindGroup->GetDescriptor();
        for (auto& entry : desc.entries) {
            Flags<ShaderStage> visibility;
            for (auto& e : bindGroupLayout->Descriptor().entries) {
                if (e.binding.binding == entry.binding) {
                    visibility = e.visibility;
                }
            }

            if (auto binding = std::get_if<TextureBinding>(&entry.entry);
                binding) {
                translateBindGroupImageLayout(visibility, binding->view);
            } else if (auto binding = std::get_if<SamplerBinding>(&entry.entry);
                binding && binding->view) {
                translateBindGroupImageLayout(visibility, binding->view);
            }
        }
        cmds_.emplace_back(cmd);
    }

    void translateBindGroupImageLayout(Flags<ShaderStage> visibility,
                                       TextureView view) {
        Assert(lastRenderPassCmdIndex_,
               "SetBindGroup() call must after BeginRenderPass()");

        auto texture = static_cast<TextureImpl*>(view.Texture().Impl());
        for (int i = 0; i < texture->layouts.size(); i++) {
            auto& layout = texture->layouts[i];
            if (layout != vk::ImageLayout::eShaderReadOnlyOptimal) {
                // do layout transform
                auto aspect = DetermineTextureAspectByFormat(texture->Format());
                vk::ImageMemoryBarrier barrier;
                vk::ImageSubresourceRange range;
                range.setAspectMask(aspect)
                    .setBaseArrayLayer(i)
                    .setLayerCount(1)
                    .setBaseMipLevel(0)
                    .setLevelCount(1);

                barrier.setOldLayout(layout)
                    .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
                    .setImage(texture->GetImage())
                    .setSubresourceRange(range);

                vk::PipelineStageFlagBits dstStage;

                CmdImageBarrier cmd;
                cmd.imageMemBarrier = barrier;
                cmd.srcStage = vk::PipelineStageFlagBits::eTransfer;
                if (visibility & ShaderStage::Vertex) {
                    dstStage = vk::PipelineStageFlagBits::eVertexInput;
                } else if (visibility & ShaderStage::Geometry) {
                    dstStage = vk::PipelineStageFlagBits::eGeometryShader;
                } else if (visibility & ShaderStage::Fragment) {
                    dstStage = vk::PipelineStageFlagBits::eFragmentShader;
                } else if (visibility & ShaderStage::Compute) {
                    dstStage = vk::PipelineStageFlagBits::eComputeShader;
                }
                cmd.dstStage = dstStage;

                layout = vk::ImageLayout::eShaderReadOnlyOptimal;

                cmds_.insert(cmds_.begin() + lastRenderPassCmdIndex_.value(),
                             std::move(cmd));

                lastRenderPassCmdIndex_.value()++;
            }
        }

        // add subpass self-dependency
        // auto& lastRenderPass = std::get<CmdBeginRenderPass>(
        //     cmds_[lastRenderPassCmdIndex_.value()]);
        // vk::SubpassDependency selfDependency =
        // lastRenderPass.analyzedRenderPassInfo.dependencies.back();
        // selfDependency.srcSubpass =
        // lastRenderPass.analyzedRenderPassInfo.subpasses.size() - 1;
        // selfDependency.dstSubpass =
        // lastRenderPass.analyzedRenderPassInfo.subpasses.size() - 1;
        // selfDependency.setDependencyFlags(vk::DependencyFlagBits::eByRegion);
        // lastRenderPass.analyzedRenderPassInfo.dependencies.emplace_back(std::move(selfDependency));
    }
};

struct CmdTranslater final {
    CmdTranslater(DeviceImpl& device, vk::CommandBuffer cmdBuf)
        : cmdBuf_{cmdBuf}, device_{device} {}

    void operator()(const CmdEndRenderPass&) { cmdBuf_.endRenderPass(); }

    void operator()(const CmdCopyBuffer2Buffer& cmd) {
        // maybe we need barrier
        vk::BufferCopy region;
        region.setSize(cmd.size)
            .setSrcOffset(cmd.srcOffset)
            .setDstOffset(cmd.dstOffset);
        cmdBuf_.copyBuffer(
            static_cast<const vulkan::BufferImpl*>(cmd.src.Impl())->buffer,
            static_cast<const vulkan::BufferImpl*>(cmd.dst.Impl())->buffer,
            region);
    }

    void operator()(const CmdImageBarrier& cmd) {
        cmdBuf_.pipelineBarrier(cmd.srcStage, cmd.dstStage,
                                vk::DependencyFlagBits::eByRegion, {}, {},
                                cmd.imageMemBarrier);
    }

    void operator()(const CmdCopyBufferToTexture& cmd) {
        vk::ImageSubresourceLayers layers;
        layers.setAspectMask(TextureAspect2Vk(cmd.dst.aspect))
            .setBaseArrayLayer(cmd.dst.origin.z)
            .setLayerCount(cmd.copySize.depthOrArrayLayers)
            .setMipLevel(cmd.dst.miplevel);
        vk::BufferImageCopy copyInfo;
        copyInfo.setBufferOffset(cmd.src.offset)
            .setImageOffset(vk::Offset3D(cmd.dst.origin.x, cmd.dst.origin.y, 0))
            .setBufferImageHeight(cmd.src.rowsPerImage)
            .setBufferRowLength(cmd.src.rowLength)
            .setImageExtent({cmd.copySize.width, cmd.copySize.height, 1})
            .setImageSubresource(layers);
        cmdBuf_.copyBufferToImage(
            static_cast<BufferImpl*>(cmd.src.buffer.Impl())->buffer,
            static_cast<TextureImpl*>(cmd.dst.texture.Impl())->GetImage(),
            vk::ImageLayout::eTransferDstOptimal, copyInfo);
    }

    void operator()(const CmdDraw& cmd) {
        if (cmd.isIndexed) {
            cmdBuf_.drawIndexed(cmd.elemCount, cmd.instanceCount,
                                cmd.firstIndex, cmd.firstVertex,
                                cmd.firstInstance);
        } else {
            cmdBuf_.draw(cmd.elemCount, cmd.instanceCount, cmd.firstVertex,
                         cmd.firstInstance);
        }
    }

    void operator()(const CmdSetVertexBuffer& cmd) {
        cmdBuf_.bindVertexBuffers(
            cmd.slot, static_cast<const BufferImpl*>(cmd.buffer.Impl())->buffer,
            cmd.offset);
    }

    void operator()(const CmdSetIndexBuffer& cmd) {
        cmdBuf_.bindIndexBuffer(
            static_cast<const BufferImpl*>(cmd.buffer.Impl())->buffer,
            cmd.offset, IndexType2Vk(cmd.type));
    }

    void operator()(const CmdSetBindGroup& cmd) {
        Assert(curPipeline_, "can't set bindgroup before set pipeline");

        auto bindGroup = static_cast<BindGroupImpl*>(cmd.bindgroup.Impl());
        cmdBuf_.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                   static_cast<const PipelineLayoutImpl*>(
                                       curPipeline_->GetLayout().Impl())
                                       ->layout,
                                   0, bindGroup->sets[device_.curImageIndex],
                                   cmd.dynamicOffsets);
    }

    void operator()(const CmdSetPushConstant& cmd) {
        Assert(curPipeline_, "can't set bindgroup before set pipeline");

        cmdBuf_.pushConstants(
            static_cast<const PipelineLayoutImpl*>(
                static_cast<const RenderPipelineImpl*>(curPipeline_)
                    ->GetLayout()
                    .Impl())
                ->layout,
            ShaderStage2Vk(cmd.stage), cmd.offset, cmd.size, cmd.value.data());
    }

    void operator()(const CmdSetViewport& cmd) {
        vk::Viewport viewport{cmd.x, cmd.y, cmd.width, cmd.height, 0, 1};
        cmdBuf_.setViewport(0, viewport);
        vk::Rect2D scissor{vk::Offset2D(cmd.x, cmd.y),
                           vk::Extent2D(cmd.width, cmd.height)};
        cmdBuf_.setScissor(0, scissor);
    }

    void operator()(const CmdSetPipeline& cmd) {
        Assert(curRenderPass_, "must begin renderpass before bind pipeline");

        auto impl = static_cast<RenderPipelineImpl*>(cmd.pipeline.Impl());
        curPipeline_ = impl;

        // TODO: change this to renderpass compatible check
        if (impl->renderPass != curRenderPass_->renderPass) {
            *impl = RenderPipelineImpl{device_, impl->GetDescriptor(),
                                       curRenderPass_->renderPass};
        }

        cmdBuf_.bindPipeline(vk::PipelineBindPoint::eGraphics,
                             curPipeline_->pipeline);
    }

    void operator()(const CmdBeginRenderPass& cmd) {
        vk::RenderPassBeginInfo beginInfo;
        auto& renderPassInfo = cmd.analyzedRenderPassInfo;
        beginInfo.setClearValues(cmd.clearValues).setRenderArea(cmd.renderArea);

        vk::RenderPassCreateInfo createInfo;
        createInfo.setAttachments(renderPassInfo.descriptions)
            .setDependencies(renderPassInfo.dependencies);

        std::vector<vk::SubpassDescription> subpasses;
        std::transform(
            renderPassInfo.subpasses.begin(), renderPassInfo.subpasses.end(),
            std::back_inserter(subpasses), [](const SubpassInfo& subpass) {
                vk::SubpassDescription description;
                description.setColorAttachments(subpass.colorRefs);
                if (subpass.depthRef) {
                    description.setPDepthStencilAttachment(
                        &subpass.depthRef.value());
                }
                return description;
            });

        createInfo.setSubpasses(subpasses);

        RenderPassImpl* renderPass{};
        for (auto&& rp : device_.renderPasses) {
            auto impl = static_cast<RenderPassImpl*>(rp.Impl());
            if (impl->renderPassInfo == renderPassInfo) {
                renderPass = impl;
            }
        }

        if (!renderPass) {
            auto rp =
                new RenderPassImpl{device_.device, cmd.desc, renderPassInfo};
            renderPass = rp;
            device_.renderPasses.emplace_back(rp);
        }

        beginInfo.setRenderPass(renderPass->renderPass);

        Framebuffer framebuffer;

        for (auto&& fbo : device_.framebuffers) {
            if (isSameFramebuffer(cmd.views, fbo)) {
                framebuffer = fbo;
                break;
            }
        }

        if (!framebuffer) {
            auto extent = cmd.views.front().Texture().Extent();
            framebuffer = device_.framebuffers.emplace_back(new FramebufferImpl{
                device_.device, cmd.views, extent, renderPass->renderPass});
        }

        beginInfo.setFramebuffer(
            static_cast<FramebufferImpl*>(framebuffer.Impl())->fbo);

        cmdBuf_.beginRenderPass(beginInfo, vk::SubpassContents::eInline);

        curRenderPass_ = renderPass;
    }

    void operator()(const CmdNextSubpass&) {
        cmdBuf_.nextSubpass(vk::SubpassContents::eInline);
    }

private:
    vk::CommandBuffer cmdBuf_;
    DeviceImpl& device_;
    RenderPipelineImpl* curPipeline_{};
    RenderPassImpl* curRenderPass_{};
};

RenderPassEncoderImpl::RenderPassEncoderImpl(CommandEncoderImpl& encoder)
    : encoder_{encoder} {}

void RenderPassEncoderImpl::Draw(uint32_t vertexCount, uint32_t instanceCount,
                                 uint32_t firstVertex, uint32_t firstInstance) {
    CmdDraw cmd;
    cmd.firstInstance = firstInstance;
    cmd.firstIndex = 0;
    cmd.isIndexed = false;
    cmd.firstVertex = firstVertex;
    cmd.elemCount = vertexCount;
    cmd.instanceCount = instanceCount;

    encoder_.cmds.push_back(cmd);
}

void RenderPassEncoderImpl::DrawIndexed(uint32_t indexCount,
                                        uint32_t instanceCount,
                                        uint32_t firstIndex,
                                        uint32_t baseVertex,
                                        uint32_t firstInstance) {
    CmdDraw cmd;
    cmd.firstInstance = firstInstance;
    cmd.firstIndex = firstIndex;
    cmd.isIndexed = true;
    cmd.firstVertex = baseVertex;
    cmd.elemCount = indexCount;
    cmd.instanceCount = instanceCount;

    encoder_.cmds.push_back(cmd);
}

void RenderPassEncoderImpl::SetVertexBuffer(uint32_t slot, Buffer buffer,
                                            uint64_t offset, uint64_t size) {
    CmdSetVertexBuffer cmd;
    cmd.slot = slot;
    cmd.buffer = buffer;
    cmd.offset = offset;
    encoder_.cmds.push_back(cmd);
}

void RenderPassEncoderImpl::SetIndexBuffer(Buffer buffer, IndexType type,
                                           uint32_t offset, uint32_t size) {
    CmdSetIndexBuffer cmd;
    cmd.buffer = buffer;
    cmd.type = type;
    cmd.offset = offset;

    encoder_.cmds.push_back(cmd);
}

void RenderPassEncoderImpl::SetBindGroup(BindGroup group) {
    CmdSetBindGroup cmd;
    cmd.bindgroup = group;

    encoder_.cmds.push_back(cmd);
}

void RenderPassEncoderImpl::SetBindGroup(
    BindGroup group, const std::vector<uint32_t>& dynamicOffset) {
    CmdSetBindGroup cmd;
    cmd.bindgroup = group;
    cmd.dynamicOffsets = dynamicOffset;

    encoder_.cmds.push_back(cmd);
}

void RenderPassEncoderImpl::SetPipeline(RenderPipeline pipeline) {
    CmdSetPipeline cmd;
    cmd.pipeline = pipeline;

    encoder_.cmds.push_back(cmd);
}

void RenderPassEncoderImpl::SetPushConstant(ShaderStage stage,
                                            const void* value, uint32_t offset,
                                            uint32_t size) {
    CmdSetPushConstant cmd;
    cmd.size = size;
    cmd.stage = stage;
    cmd.offset = offset;
    if (offset + size > cmd.value.size()) {
        LOGE(log_tag::Vulkan, "push constant max size is ", cmd.value.size(),
             "bytes, your end byte is ", offset + size);
    }
    memcpy(cmd.value.data(), value, size);

    encoder_.cmds.push_back(cmd);
}

void RenderPassEncoderImpl::SetViewport(float x, float y, float width,
                                        float height) {
    CmdSetViewport cmd;
    cmd.x = x;
    cmd.y = y;
    cmd.width = width;
    cmd.height = height;

    encoder_.cmds.push_back(cmd);
}

void RenderPassEncoderImpl::End() {
    encoder_.cmds.push_back(CmdEndRenderPass{});
}

RenderPassEncoder CommandEncoderImpl::BeginRenderPass(
    const RenderPass::Descriptor& desc) {
    CmdBeginRenderPass cmd;
    cmd.desc = desc;
    cmds.push_back(cmd);

    return RenderPassEncoder{new RenderPassEncoderImpl{*this}};
}

CommandEncoderImpl::CommandEncoderImpl(DeviceImpl& dev, vk::CommandPool pool)
    : dev_{dev}, pool{pool} {
    vk::CommandBufferAllocateInfo info;
    info.setCommandPool(pool).setCommandBufferCount(1).setLevel(
        vk::CommandBufferLevel::ePrimary);

    auto buf = dev.RequireCmdBuf();
    cmdBuf = CommandBufferImpl(buf);

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    VK_CALL_NO_VALUE(buf.begin(beginInfo));
}

CommandEncoderImpl::~CommandEncoderImpl() {
    if (pool) {
        dev_.ResetCmdBuf(cmdBuf.buf);
    }
}

void CommandEncoderImpl::CopyBufferToBuffer(const Buffer& src,
                                            uint64_t srcOffset,
                                            const Buffer& dst,
                                            uint64_t dstOffset, uint64_t size) {
    CmdCopyBuffer2Buffer cmd;
    cmd.src = src;
    cmd.dst = dst;
    cmd.srcOffset = srcOffset;
    cmd.dstOffset = dstOffset;
    cmd.size = size;
    cmds.push_back(cmd);
}

void CommandEncoderImpl::CopyBufferToTexture(
    const CommandEncoder::BufTexCopySrc& src,
    const CommandEncoder::BufTexCopyDst& dst, const Extent3D& copySize) {
    CmdCopyBufferToTexture cmd;
    cmd.src = src;
    cmd.dst = dst;
    cmd.copySize = copySize;
    cmds.push_back(cmd);
}

CommandBuffer CommandEncoderImpl::Finish() {
    CmdAnalyzer analyzer{*this, dev_};
    auto& cmds = analyzer.Analyze();

    CmdTranslater translater{dev_, cmdBuf.buf};

    for (auto&& cmd : cmds) {
        std::visit(translater, cmd);
    }

    static_cast<CommandBufferImpl&>(cmdBuf).needWaitImageAvaliFence =
        analyzer.HasPresent();

    VK_CALL_NO_VALUE(cmdBuf.buf.end());
    return CommandBuffer{&cmdBuf};
}

}  // namespace nickel::rhi::vulkan