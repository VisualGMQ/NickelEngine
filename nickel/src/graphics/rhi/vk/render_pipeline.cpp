#include "graphics/rhi/vk/render_pipeline.hpp"
#include "graphics/rhi/vk/convert.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/pipeline_layout.hpp"
#include "graphics/rhi/vk/shader.hpp"

namespace nickel::rhi::vulkan {

RenderPipelineImpl::RenderPipelineImpl(DeviceImpl& dev,
                                       const RenderPipeline::Descriptor& desc)
    : dev_{dev.device}, layout_{desc.layout}, desc_{desc} {
    createRenderPass(dev, desc);
    createRenderPipeline(dev, desc);
}

RenderPipelineImpl::RenderPipelineImpl(DeviceImpl& dev,
                                       const RenderPipeline::Descriptor& desc,
                                       vk::RenderPass renderPass)
    : renderPass{renderPass},
      dev_{dev.device},
      layout_{desc.layout},
      desc_{desc} {
    createRenderPipeline(dev, desc);
}

RenderPipelineImpl::RenderPipelineImpl(RenderPipelineImpl&& o) {
    swap(*this, o);
}

RenderPipelineImpl& RenderPipelineImpl::operator=(RenderPipelineImpl&& o) {
    swap(*this, o);
    return *this;
}

const RenderPipeline::Descriptor& RenderPipelineImpl::GetDescriptor() const {
    return desc_;
}

void RenderPipelineImpl::createRenderPipeline(
    DeviceImpl& dev, const RenderPipeline::Descriptor& desc) {
    vk::GraphicsPipelineCreateInfo info;

    // vertex input state
    auto& vertexInputState = desc.vertex;
    vk::PipelineVertexInputStateCreateInfo vertexInput;
    std::vector<vk::VertexInputAttributeDescription> attrDescs;
    std::vector<vk::VertexInputBindingDescription> bindings;
    for (int i = 0; i < vertexInputState.buffers.size(); i++) {
        auto& state = vertexInputState.buffers[i];
        for (auto& attr : state.attributes) {
            vk::VertexInputAttributeDescription attrDesc;
            attrDesc.setBinding(i)
                .setLocation(attr.shaderLocation)
                .setFormat(VertexFormat2Vk(attr.format))
                .setOffset(attr.offset);
            attrDescs.emplace_back(attrDesc);
        }

        vk::VertexInputBindingDescription binding;
        binding.setBinding(i)
            .setStride(state.arrayStride)
            .setInputRate(state.stepMode ==
                                  RenderPipeline::BufferState::StepMode::Vertex
                              ? vk::VertexInputRate::eVertex
                              : vk::VertexInputRate::eInstance);
        bindings.emplace_back(binding);
    }

    vertexInput.setVertexAttributeDescriptions(attrDescs)
        .setVertexBindingDescriptions(bindings);

    // input assembly state
    vk::PipelineInputAssemblyStateCreateInfo inputAsm;
    inputAsm
        .setPrimitiveRestartEnable(false)  // doubt!
        .setTopology(Topology2Vk(desc.primitive.topology));

    // shader stage
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    shaderStages.reserve(2);
    if (desc.vertex.module) {
        shaderStages.resize(1);
        shaderStages[0]
            .setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(static_cast<const ShaderModuleImpl*>(
                           desc.vertex.module.Impl().get())
                           ->module)
            .setPName(desc.vertex.entryPoint.c_str());
    }
    if (desc.fragment.module) {
        shaderStages.resize(2);
        shaderStages[1]
            .setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(static_cast<const ShaderModuleImpl*>(
                           desc.fragment.module.Impl().get())
                           ->module)
            .setPName(desc.fragment.entryPoint.c_str());
        if (desc.geometry) {
            shaderStages.resize(3);
            shaderStages[2]
                .setStage(vk::ShaderStageFlagBits::eGeometry)
                .setModule(static_cast<const ShaderModuleImpl*>(
                               desc.geometry->module.Impl().get())
                               ->module)
                .setPName(desc.fragment.entryPoint.c_str());
        }
    }

    // viewport
    vk::PipelineViewportStateCreateInfo viewportState;
    vk::Viewport viewport{desc.viewport.viewport.x,
                          desc.viewport.viewport.y,
                          desc.viewport.viewport.w,
                          desc.viewport.viewport.h,
                          0,
                          1};
    vk::Rect2D scissor{
        {    desc.viewport.scissor.offset.x,desc.viewport.scissor.offset.y             },
        {desc.viewport.scissor.extent.width,
         desc.viewport.scissor.extent.height}
    };
    viewportState.setViewports(viewport).setScissors(scissor);

    // rasterization
    vk::PipelineRasterizationStateCreateInfo rasterState;
    rasterState.setRasterizerDiscardEnable(false);

    if (desc.depthStencil) {
        rasterState.setDepthBiasEnable(desc.depthStencil->depthBias.has_value())
            .setDepthClampEnable(desc.depthStencil->depthBiasClamp.has_value())
            .setDepthBiasSlopeFactor(desc.depthStencil->depthBiasSlopeScale);

        if (desc.depthStencil->depthBias) {
            rasterState.setDepthBiasConstantFactor(
                desc.depthStencil->depthBias.value());
        }

        if (desc.depthStencil->depthBiasClamp) {
            rasterState.setDepthBiasClamp(
                desc.depthStencil->depthBiasClamp.value());
        }
    }

    rasterState.setCullMode(CullMode2Vk(desc.primitive.cullMode))
        .setPolygonMode(PolygonMode2Vk(desc.primitive.polygonMode))
        .setLineWidth(1)
        .setFrontFace(FrontFace2Vk(desc.primitive.frontFace));

    // multisample
    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample
        .setAlphaToCoverageEnable(desc.multisample.alphaToCoverageEnabled)
        .setAlphaToOneEnable(desc.multisample.alphaToCoverageEnabled)
        .setSampleShadingEnable(false)
        .setRasterizationSamples(SampleCount2Vk(desc.multisample.count))
        .setPSampleMask(&desc.multisample.mask);

    // color blend
    std::vector<vk::PipelineColorBlendAttachmentState> attachmentStates;
    for (auto& target : desc.fragment.targets) {
        vk::PipelineColorBlendAttachmentState colorState;
        colorState.setColorBlendOp(BlendOp2Vk(target.blend.color.operation))
            .setSrcColorBlendFactor(
                BlendFactor2Vk(target.blend.color.srcFactor))
            .setDstColorBlendFactor(
                BlendFactor2Vk(target.blend.color.dstFactor))
            .setAlphaBlendOp(BlendOp2Vk(target.blend.alpha.operation))
            .setSrcAlphaBlendFactor(
                BlendFactor2Vk(target.blend.alpha.srcFactor))
            .setDstAlphaBlendFactor(
                BlendFactor2Vk(target.blend.alpha.dstFactor))
            .setBlendEnable(true)
            .setColorWriteMask(ColorWriteMask2Vk(target.writeMask));
        attachmentStates.emplace_back(colorState);
    }

    vk::PipelineColorBlendStateCreateInfo colorBlend;
    colorBlend.setLogicOpEnable(false).setAttachments(attachmentStates);

    // depth stencil state
    vk::PipelineDepthStencilStateCreateInfo depthStencilState;
    if (desc.depthStencil) {
        vk::StencilOpState frontState, backState;
        frontState.setWriteMask(desc.depthStencil->stencilWriteMask)
            .setCompareMask(desc.depthStencil->stencilReadMask)
            .setCompareOp(CompareOp2Vk(desc.depthStencil->stencilFront.compare))
            .setFailOp(StencilOp2Vk(desc.depthStencil->stencilFront.failedOp))
            .setPassOp(StencilOp2Vk(desc.depthStencil->stencilFront.passOp))
            .setDepthFailOp(
                StencilOp2Vk(desc.depthStencil->stencilFront.depthFailOp))
            .setReference(0);
        backState.setWriteMask(desc.depthStencil->stencilWriteMask)
            .setCompareMask(desc.depthStencil->stencilReadMask)
            .setCompareOp(CompareOp2Vk(desc.depthStencil->stencilBack.compare))
            .setFailOp(StencilOp2Vk(desc.depthStencil->stencilBack.failedOp))
            .setPassOp(StencilOp2Vk(desc.depthStencil->stencilBack.passOp))
            .setDepthFailOp(
                StencilOp2Vk(desc.depthStencil->stencilBack.depthFailOp))
            .setReference(0);

        depthStencilState.sType =
            vk::StructureType::ePipelineDepthStencilStateCreateInfo;
        depthStencilState.setDepthBoundsTestEnable(false)
            .setDepthTestEnable(true)
            .setStencilTestEnable(true)
            .setDepthWriteEnable(desc.depthStencil->depthWriteEnabled)
            .setDepthCompareOp(CompareOp2Vk(desc.depthStencil->depthCompare))
            .setFront(frontState)
            .setBack(backState);

        info.setPDepthStencilState(&depthStencilState);
    }

    // create render pipeline
    info.setPVertexInputState(&vertexInput)
        .setPInputAssemblyState(&inputAsm)
        .setStages(shaderStages)
        .setPViewportState(&viewportState)
        .setPRasterizationState(&rasterState)
        .setPMultisampleState(&multisample)
        .setPColorBlendState(&colorBlend)
        .setLayout(
            static_cast<const PipelineLayoutImpl*>(desc.layout.Impl())->layout)
        .setRenderPass(renderPass);

    VK_CALL(pipeline, dev.device.createGraphicsPipeline(nullptr, info));

    // clear shader module
    auto module = desc.vertex.module;
    module.Destroy();
    module = desc.fragment.module;
    module.Destroy();
    if (desc.geometry) {
        module = desc.geometry->module;
        module.Destroy();
    }
}

void RenderPipelineImpl::createRenderPass(
    DeviceImpl& dev, const RenderPipeline::Descriptor& desc) {
    vk::RenderPassCreateInfo info;

    std::vector<vk::AttachmentDescription> attachments;
    size_t attCount =
        desc.fragment.targets.size() + (desc.depthStencil.has_value() ? 1 : 0);
    attachments.reserve(attCount);
    std::vector<vk::AttachmentReference> colorAttRefs;
    colorAttRefs.reserve(desc.fragment.targets.size());

    for (int i = 0; i < desc.fragment.targets.size(); i++) {
        auto& target = desc.fragment.targets[i];

        vk::AttachmentDescription attDesc;
        attDesc.setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(GetLayoutByFormat(target.format))
            .setFormat(target.format == TextureFormat::Presentation
                           ? dev.swapchain.imageInfo.format.format
                           : TextureFormat2Vk(target.format))
            .setLoadOp(AttachmentLoadOp2Vk(AttachmentLoadOp::Clear))
            .setStoreOp(AttachmentStoreOp2Vk(AttachmentStoreOp::Store))
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setSamples(SampleCount2Vk(desc.multisample.count));

        attachments.push_back(attDesc);

        vk::AttachmentReference ref;
        ref.setAttachment(i).setLayout(
            vk::ImageLayout::eColorAttachmentOptimal);
        colorAttRefs.emplace_back(ref);
    }

    vk::AttachmentReference depthAttRef;
    if (desc.depthStencil) {
        vk::AttachmentDescription depthAttach;

        depthAttach.setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
            .setFormat(TextureFormat2Vk(desc.depthStencil->depthFormat))
            .setLoadOp(AttachmentLoadOp2Vk(AttachmentLoadOp::Clear))
            .setStoreOp(AttachmentStoreOp2Vk(AttachmentStoreOp::Discard))
            .setStencilLoadOp(AttachmentLoadOp2Vk(AttachmentLoadOp::Clear))
            .setStencilStoreOp(AttachmentStoreOp2Vk(AttachmentStoreOp::Discard))
            .setSamples(vk::SampleCountFlagBits::e1);

        attachments.push_back(depthAttach);

        auto viewFmt = desc.depthStencil->depthFormat;
        depthAttRef
            .setAttachment(desc.fragment.targets.size())
            //.setLayout(GetLayoutByFormat(
            //    viewFmt, desc.depthStencil->stencilReadMask == 0,
            //    !desc.depthStencil->depthWriteEnabled));
            .setLayout(desc.depthStencil->depthWriteEnabled
                           ? vk::ImageLayout::eDepthStencilAttachmentOptimal
                           : vk::ImageLayout::eDepthStencilReadOnlyOptimal);
    }

    vk::SubpassDescription subpass;
    subpass.setColorAttachments(colorAttRefs)
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    if (desc.depthStencil) {
        subpass.setPDepthStencilAttachment(&depthAttRef);
    }

    vk::SubpassDependency dep;
    dep.setSrcSubpass(VK_SUBPASS_EXTERNAL).setDstSubpass(0);
    if (desc.depthStencil) {
        dep.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                            vk::PipelineStageFlagBits::eEarlyFragmentTests)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput |
                             vk::PipelineStageFlagBits::eEarlyFragmentTests)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite |
                              vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    } else {
        dep.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
    }

    info.setAttachments(attachments).setSubpasses(subpass).setDependencies(dep);

    VK_CALL(renderPass, dev.device.createRenderPass(info));
}

RenderPipelineImpl::~RenderPipelineImpl() {
    for (auto module : shaderModules_) {
        dev_.destroyShaderModule(module);
    }
    if (renderPass) {
        dev_.destroyRenderPass(renderPass);
    }
    if (pipeline) {
        dev_.destroyPipeline(pipeline);
    }
}

}  // namespace nickel::rhi::vulkan