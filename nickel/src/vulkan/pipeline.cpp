#include "vulkan/pipeline.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"

namespace nickel::vulkan {

GraphicsPipeline::GraphicsPipeline(
    Device* device, const VertexLayout& vertexLayout,
    const vk::PipelineInputAssemblyStateCreateInfo& inputAsm,
    const std::vector<ShaderModule*>& shaders,
    const vk::PipelineViewportStateCreateInfo& viewport,
    const vk::PipelineRasterizationStateCreateInfo& raster,
    const vk::PipelineDepthStencilStateCreateInfo& depthStencil,
    const vk::PipelineMultisampleStateCreateInfo& multisample,
    const vk::PipelineColorBlendStateCreateInfo& colorBlend,
    vk::PipelineLayout layout, vk::RenderPass renderPass)
    : device_{device} {
    vk::GraphicsPipelineCreateInfo createInfo;

    auto bindingDesc = CvtLayout2BindingDescription(vertexLayout);
    auto attribDesc = CvtLayout2AttribDescription(vertexLayout);
    vk::PipelineVertexInputStateCreateInfo inputState({}, bindingDesc,
                                                      attribDesc);
    createInfo.setPVertexInputState(&inputState);

    createInfo.setPInputAssemblyState(&inputAsm);

    std::vector<vk::PipelineShaderStageCreateInfo> stages;
    for (auto& shader : shaders) {
        stages.emplace_back(shader->GetShaderStageCreateInfo());
    }
    createInfo.setStages(stages);

    createInfo.setPViewportState(&viewport)
        .setPRasterizationState(&raster)
        .setPDepthStencilState(&depthStencil)
        .setPMultisampleState(&multisample)
        .setPColorBlendState(&colorBlend)
        .setLayout(layout)
        .setRenderPass(renderPass);

    VK_CALL(pipeline_,
            device->GetDevice().createGraphicsPipeline(nullptr, createInfo));
}

GraphicsPipeline::~GraphicsPipeline() {
    if (device_) {
        device_->GetDevice().destroyPipeline(pipeline_);
    }
}

PipelineLayout::PipelineLayout(
    Device* device, const std::vector<vk::DescriptorSetLayout>& layouts,
    const std::vector<vk::PushConstantRange>& pushConstantRanges)
    : device_{device} {
    vk::PipelineLayoutCreateInfo info;
    info.setPushConstantRanges(pushConstantRanges).setSetLayouts(layouts);
    VK_CALL(layout_, device->GetDevice().createPipelineLayout(info));
}

PipelineLayout::~PipelineLayout() {
    if (device_) {
        device_->GetDevice().destroyPipelineLayout(layout_);
    }
}

}  // namespace nickel::vulkan