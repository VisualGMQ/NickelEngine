#include "rhi/vulkan/pipeline.hpp"
#include "rhi/vulkan/device.hpp"

namespace nickel::rhi::vulkan {

vk::Format cvtAttribFormat(Attribute::Type type) {
    switch (type) {
        case Attribute::Type::Float:
            return vk::Format::eR32Sfloat;
        case Attribute::Type::Vec2:
            return vk::Format::eR32G32Sfloat;
        case Attribute::Type::Vec3:
            return vk::Format::eR32G32B32Sfloat;
        case Attribute::Type::Vec4:
            return vk::Format::eR32G32B32A32Sfloat;
    }
}

std::vector<vk::VertexInputAttributeDescription> CvtLayout2AttribDescription(
    const VertexLayout& layout) {
    std::vector<vk::VertexInputAttributeDescription> descs;

    auto& attrs = layout.Attributes();
    for (int i = 0; i < attrs.size(); i++) {
        auto& attr = attrs[i];
        vk::VertexInputAttributeDescription desc;
        desc.setBinding(attr.location)
            .setFormat(cvtAttribFormat(attr.type))
            .setBinding(0);

        descs.push_back(desc);
    }

    return descs;
}

std::vector<vk::VertexInputBindingDescription> CvtLayout2BindingDescription(
    const VertexLayout& layout) {
    auto& attrs = layout.Attributes();

    std::vector<vk::VertexInputBindingDescription> descs;
    for (auto& attr : attrs) {
        vk::VertexInputBindingDescription desc;
        desc.setInputRate(vk::VertexInputRate::eVertex)
            .setBinding(0)
            .setStride(layout.Stride());
        descs.push_back(desc);
    }
    return descs;
}

PipelineLayout::PipelineLayout(
    Device* device, const std::vector<DescriptorSetLayout*>& layouts,
    std::optional<PushConstantRange> pushConstantRange)
    : device_(device) {
    vk::PipelineLayoutCreateInfo createInfo;
    std::vector<vk::DescriptorSetLayout> setLayouts;
    for (auto& layout : layouts) {
        setLayouts.push_back(layout->Raw());
    }
    createInfo.setSetLayouts(setLayouts);
    if (pushConstantRange) {
        vk::PushConstantRange range;
        range = PushConstantRange2Vk(pushConstantRange.value());
        createInfo.setPushConstantRanges(range);
    }
    layout_ = device_->Raw().createPipelineLayout(createInfo);
    if (!layout_) {
        LOGW(log_tag::Vulkan, "create pipeline layout failed");
    }
}

PipelineLayout::~PipelineLayout() {
    if (device_ && layout_) {
        device_->Raw().destroyPipelineLayout(layout_);
    }
}

GraphicsPipeline::GraphicsPipeline(const VertexLayout& vertexLayout,
                                   const InputAssemblyState& assemblyState,
                                   const std::vector<ShaderModule*>& shaders,
                                   const ViewportState& viewportState,
                                   const RasterizationState& rasterState,
                                   const DepthStencilState& depthStencilState,
                                   const MultisampleState& multisampleState,
                                   const ColorBlendState& colorBlendState,
                                   const PipelineLayout& layout) {
    vk::GraphicsPipelineCreateInfo createInfo;

    // config vertex input
    auto bindingDescs = CvtLayout2BindingDescription(vertexLayout);
    auto attribDescs = CvtLayout2AttribDescription(vertexLayout);
    vk::PipelineVertexInputStateCreateInfo vertexInputState;
    vertexInputState.setVertexAttributeDescriptions(attribDescs)
        .setVertexBindingDescriptions(bindingDescs);
    createInfo.setPVertexInputState(&vertexInputState);

    // input assembly
    vk::PipelineInputAssemblyStateCreateInfo asmStateInfo;
    asmStateInfo.setTopology(Primitive2Vk(assemblyState.primitive))
        .setPrimitiveRestartEnable(assemblyState.primitiveRestartEnable);
    createInfo.setPInputAssemblyState(&asmStateInfo);

    // shader module
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStageInfos;
    for (auto& shader : shaders) {
        shaderStageInfos.push_back(shader->GetShaderStageCreateInfo());
    }
    createInfo.setStages(shaderStageInfos);

    // viewport state
    vk::PipelineViewportStateCreateInfo viewport;
    std::vector<vk::Viewport> viewports;
    for (auto& v : viewportState.viewports) {
        viewports.emplace_back(v.x, v.y, v.width, v.height, v.minDepth,
                               v.maxDepth);
    }
    std::vector<vk::Rect2D> scissors;
    for (auto& s : viewportState.sissors) {
        scissors.emplace_back(vk::Offset2D{s.x, s.y},
                              vk::Extent2D{s.width, s.height});
    }
    viewport.setViewports(viewports);
    viewport.setScissors(scissors);
    createInfo.setPViewportState(&viewport);

    // rasterization state
    vk::PipelineRasterizationStateCreateInfo rasterInfo;
    rasterInfo.setDepthClampEnable(rasterState.depthClampEnable)
        .setRasterizerDiscardEnable(rasterState.discardEnable)
        .setPolygonMode(PolygonMode2Vk(rasterState.polygonMode))
        .setCullMode(CullMode2Vk(rasterState.cullMode))
        .setFrontFace(FrontFace2Vk(rasterState.frontFace))
        .setDepthBiasEnable(rasterState.depthBiasEnable)
        .setDepthBiasConstantFactor(rasterState.depthBiasConstantFactor)
        .setDepthBiasClamp(rasterState.depthBiasClamp)
        .setDepthBiasSlopeFactor(rasterState.depthBiasSlopeFactor)
        .setLineWidth(rasterState.lineWidth);
    createInfo.setPRasterizationState(&rasterInfo);

    // multisample
    vk::PipelineMultisampleStateCreateInfo multisampleInfo;
    std::vector<vk::SampleMask> masks;
    for (auto& mask : multisampleState.sampleMasks) {
        masks.emplace_back(mask);
    }
    multisampleInfo.setMinSampleShading(multisampleState.minSampleShading)
        .setRasterizationSamples(
            SampleCountFlag2Vk(multisampleState.sampleCount))
        .setSampleShadingEnable(multisampleState.sampleShadingEnable)
        .setAlphaToCoverageEnable(multisampleState.alphaToCoverageEnable)
        .setAlphaToOneEnable(multisampleState.alphaToOneEnable)
        .setPSampleMask(masks.empty() ? nullptr : masks.data());
    createInfo.setPMultisampleState(&multisampleInfo);

    // depth stencil test
    vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
    depthStencilInfo.setDepthTestEnable(depthStencilState.depthTestEnable)
        .setDepthWriteEnable(depthStencilState.depthWriteEnable)
        .setDepthCompareOp(CompareOp2Vk(depthStencilState.depthCompare))
        .setDepthBoundsTestEnable(depthStencilState.depthBoundsTestEnable)
        .setStencilTestEnable(depthStencilState.stencilTestEnable)
        .setFront(StencilOp2Vk(depthStencilState.front))
        .setBack(StencilOp2Vk(depthStencilState.back))
        .setMinDepthBounds(depthStencilState.minDepthBound)
        .setMaxDepthBounds(depthStencilState.maxDepthBound);
    createInfo.setPDepthStencilState(&depthStencilInfo);

    // color blend
    vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
    std::vector<vk::PipelineColorBlendAttachmentState> attachmentStates;
    for (auto& att : colorBlendState.attachmentStates) {
        vk::PipelineColorBlendAttachmentState state;
        state.setColorBlendOp(BlendOp2Vk(att.colorBlendOp))
            .setAlphaBlendOp(BlendOp2Vk(att.alphaBlendOp))
            .setSrcColorBlendFactor(BlendFactor2Vk(att.srcColorBlendFactor))
            .setDstColorBlendFactor(BlendFactor2Vk(att.dstColorBlendFactor))
            .setSrcAlphaBlendFactor(BlendFactor2Vk(att.srcAlphaBlendFactor))
            .setDstAlphaBlendFactor(BlendFactor2Vk(att.dstAlphaBlendFactor))
            .setBlendEnable(att.blendEnable);
    }
    colorBlendInfo.setLogicOp(LogicOp2Vk(colorBlendState.logicOp))
        .setLogicOpEnable(colorBlendState.logicOpEnable)
        .setBlendConstants(colorBlendState.blendConstants)
        .setAttachments(attachmentStates);
    createInfo.setPColorBlendState(&colorBlendInfo);

    // layout info
    createInfo.setLayout(layout.Raw());

    auto result = device_->Raw().createGraphicsPipeline(nullptr, createInfo);
    if (result.result != vk::Result::eSuccess) {
        LOGW(log_tag::Vulkan,
             "create graphics pipeline failed: ", result.result);
    } else {
        pipeline_ = result.value;
    }
}

GraphicsPipeline::~GraphicsPipeline() {
    if (pipeline_ && device_) {
        device_->Raw().destroyPipeline(pipeline_);
    }
}

}  // namespace nickel::rhi::vulkan