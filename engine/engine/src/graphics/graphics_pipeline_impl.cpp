#include "nickel/graphics/internal/graphics_pipeline_impl.hpp"
#include "nickel/graphics/graphics_pipeline.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/enum_convert.hpp"
#include "nickel/graphics/internal/render_pass_impl.hpp"
#include "nickel/graphics/internal/shader_module_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

nickel::graphics::GraphicsPipelineImpl::GraphicsPipelineImpl(
    DeviceImpl& dev, const GraphicsPipeline::Descriptor& desc)
    : m_layout{desc.layout}, m_device{dev}, m_render_pass{desc.m_render_pass} {
    VkGraphicsPipelineCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    ci.subpass = 0;

    // shader stage
    {
        std::vector<VkPipelineShaderStageCreateInfo> stage_ci_list;
        stage_ci_list.reserve(desc.m_shader_stages.size());
        for (auto&& [stage, module] : desc.m_shader_stages) {
            VkPipelineShaderStageCreateInfo ci{};
            ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            ci.stage = static_cast<VkShaderStageFlagBits>(ShaderStage2Vk(stage));
            ci.pName = module.entry_name.c_str();
            ci.module = module.module.Impl().m_module;
            stage_ci_list.push_back(ci);
        }

        ci.stageCount = stage_ci_list.size();
        ci.pStages = stage_ci_list.data();
    }

    // vertex input state
    auto& vertexInputState = desc.vertex;
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    std::vector<VkVertexInputAttributeDescription> attrDescs;
    std::vector<VkVertexInputBindingDescription> bindings;
    for (int i = 0; i < vertexInputState.buffers.size(); i++) {
        auto& state = vertexInputState.buffers[i];
        for (auto& attr : state.attributes) {
            VkVertexInputAttributeDescription attrDesc;
            attrDesc.binding = i;
            attrDesc.location = attr.shaderLocation;
            attrDesc.format = VertexFormat2Vk(attr.format);
            attrDesc.offset = attr.offset;
            attrDescs.emplace_back(attrDesc);
        }

        VkVertexInputBindingDescription binding;
        binding.binding = i;
        binding.stride = state.arrayStride;
        binding.inputRate =
            state.stepMode ==
                    GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex
                ? VK_VERTEX_INPUT_RATE_VERTEX
                : VK_VERTEX_INPUT_RATE_INSTANCE;
        bindings.emplace_back(binding);
    }
    vertexInput.vertexAttributeDescriptionCount = attrDescs.size();
    vertexInput.pVertexAttributeDescriptions = attrDescs.data();
    vertexInput.vertexAttributeDescriptionCount = bindings.size();
    vertexInput.pVertexBindingDescriptions = bindings.data();

    // input assembly state
    VkPipelineInputAssemblyStateCreateInfo inputAsm{};
    inputAsm.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAsm.topology = Topology2Vk(desc.primitive.topology);
    inputAsm.primitiveRestartEnable = false;  // TODO: read from config later

    // rasterization
    VkPipelineRasterizationStateCreateInfo rasterState{};
    rasterState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterState.rasterizerDiscardEnable =
        false;  // TODO: read from config later

    if (desc.depthStencil) {
        rasterState.depthBiasEnable = desc.depthStencil->depthBias.has_value();
        rasterState.depthClampEnable =
            desc.depthStencil->depthBiasClamp.has_value();
        rasterState.depthBiasSlopeFactor =
            desc.depthStencil->depthBiasSlopeScale;

        if (desc.depthStencil->depthBias) {
            rasterState.depthBiasConstantFactor =
                desc.depthStencil->depthBias.value();
        }

        if (desc.depthStencil->depthBiasClamp) {
            rasterState.depthBiasClamp =
                desc.depthStencil->depthBiasClamp.value();
        }
    }

    rasterState.cullMode = CullMode2Vk(desc.primitive.cullMode);
    rasterState.polygonMode = PolygonMode2Vk(desc.primitive.polygonMode);
    rasterState.lineWidth = 1.0f;  // TODO: read from config later
    rasterState.frontFace = FrontFace2Vk(desc.primitive.frontFace);

    // multisample
    VkPipelineMultisampleStateCreateInfo multisample{};
    multisample.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.alphaToCoverageEnable = desc.multisample.alphaToCoverageEnabled;
    multisample.sampleShadingEnable = false;
    multisample.rasterizationSamples = static_cast<VkSampleCountFlagBits>(
        SampleCount2Vk(desc.multisample.count));
    multisample.pSampleMask = &desc.multisample.mask;

     // color blend
    std::vector<VkPipelineColorBlendAttachmentState> attachmentStates;
    for (auto& state : desc.blend_state) {
        VkPipelineColorBlendAttachmentState colorState;
        colorState.colorBlendOp = BlendOp2Vk(state.color.operation);
        colorState.srcColorBlendFactor =
            BlendFactor2Vk(state.color.srcFactor);
        colorState.dstColorBlendFactor = BlendFactor2Vk(state.color.dstFactor);
        colorState.alphaBlendOp = BlendOp2Vk(state.alpha.operation);
        colorState.srcAlphaBlendFactor = BlendFactor2Vk(state.alpha.srcFactor);
        colorState.dstAlphaBlendFactor = BlendFactor2Vk(state.alpha.dstFactor);
        colorState.blendEnable = true;
        colorState.colorWriteMask = ColorComponent2Vk(state.colorMask);
        attachmentStates.emplace_back(colorState);
    }

    VkPipelineColorBlendStateCreateInfo colorBlend;
    colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.logicOpEnable = false;
    colorBlend.attachmentCount = attachmentStates.size();
    colorBlend.pAttachments = attachmentStates.data();

    // depth stencil state
    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    depthStencilState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    if (desc.depthStencil) {
        VkStencilOpState frontState, backState;
        frontState.writeMask = desc.depthStencil->stencilFront.writeMask;
        frontState.compareMask = desc.depthStencil->stencilFront.compareMask;
        frontState.compareOp =
            CompareOp2Vk(desc.depthStencil->stencilFront.compare);
        frontState.failOp =
            StencilOp2Vk(desc.depthStencil->stencilFront.failedOp);
        frontState.passOp =
            StencilOp2Vk(desc.depthStencil->stencilFront.passOp);
        frontState.depthFailOp =
            StencilOp2Vk(desc.depthStencil->stencilFront.depthFailOp);
        frontState.reference = 0;

        backState.writeMask = desc.depthStencil->stencilBack.writeMask;
        backState.compareMask = desc.depthStencil->stencilBack.compareMask;
        backState.compareOp =
            CompareOp2Vk(desc.depthStencil->stencilBack.compare);
        backState.failOp =
            StencilOp2Vk(desc.depthStencil->stencilBack.failedOp);
        backState.passOp = StencilOp2Vk(desc.depthStencil->stencilBack.passOp);
        backState.depthFailOp =
            StencilOp2Vk(desc.depthStencil->stencilBack.depthFailOp);
        backState.reference = 0;

        depthStencilState.depthBoundsTestEnable = false;
        depthStencilState.depthWriteEnable =
            desc.depthStencil->depthWriteEnabled;
        depthStencilState.depthTestEnable = true;
        depthStencilState.stencilTestEnable = true;
        depthStencilState.depthCompareOp =
            CompareOp2Vk(desc.depthStencil->depthCompare);
        depthStencilState.front = frontState;
        depthStencilState.back = backState;

        ci.pDepthStencilState = &depthStencilState;
    }

    // dynamic states
    VkPipelineDynamicStateCreateInfo dynState;
    dynState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    std::array states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    dynState.dynamicStateCount = states.size();
    dynState.pDynamicStates = states.data();

    // temporary viewport
    VkPipelineViewportStateCreateInfo viewportState;
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    VkViewport viewport{};
    VkRect2D scissor{};
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // create pipeline
    ci.pDepthStencilState = &depthStencilState;
    ci.renderPass = desc.m_render_pass.Impl().m_render_pass;
    ci.pVertexInputState = &vertexInput;
    ci.pInputAssemblyState = &inputAsm;
    ci.pTessellationState = nullptr;
    ci.pViewportState = &viewportState;
    ci.pRasterizationState = &rasterState;
    ci.pMultisampleState = &multisample;
    ci.pColorBlendState = &colorBlend;
    ci.pDynamicState = &dynState;
    ci.layout = desc.layout.Impl().m_pipeline_layout;

    VK_CALL(vkCreateGraphicsPipelines(dev.m_device, VK_NULL_HANDLE, 1, &ci,
                                      nullptr, &m_pipeline));
}

nickel::graphics::GraphicsPipelineImpl::~GraphicsPipelineImpl() {
    vkDestroyPipeline(m_device.m_device, m_pipeline, nullptr);
}

void nickel::graphics::GraphicsPipelineImpl::Release() {
    m_layout.Release();
}